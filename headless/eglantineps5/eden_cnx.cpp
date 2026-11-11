#include "eden_services.h"

#include "assets_dir.h"
#include "remote/http.h"

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <exception>
#include <memory>

namespace {

Eden::CNX::Fetch FetchTranslations(const std::atomic<bool>& stop) {
    return [&stop](const std::string& url, const std::filesystem::path& destination, std::string& error) {
        struct Transfer {
            FILE* file;
            const std::atomic<bool>& stop;
            std::uint64_t received = 0;
            std::uint64_t limit;
        };
        std::unique_ptr<FILE, decltype(&std::fclose)> file(std::fopen(destination.c_str(), "wb"), &std::fclose);
        if (!file) {
            error = "Não foi possível salvar o download da tradução.";
            return false;
        }
        const bool catalog = url == Eden::CNX::kCatalogUrl;
        Transfer transfer{file.get(), stop, 0, catalog ? 4ull << 20 : 8ull << 30};
        remote_http_request request{};
        request.url = url.c_str();
        request.https_only = 1;
        request.timeout_ms = catalog ? 30000 : 0;
        request.raw = catalog ? 0 : 1;
        request.user = &transfer;
        request.begin = [](void* user, int status, std::uint64_t length) {
            return status == 200 && length <= static_cast<Transfer*>(user)->limit ? 1 : 0;
        };
        request.stop = [](void* user) { return static_cast<Transfer*>(user)->stop.load() ? 1 : 0; };
        request.sink = [](void* user, const void* bytes, std::size_t size) {
            auto& state = *static_cast<Transfer*>(user);
            if (state.stop || size > state.limit - state.received) return 0;
            state.received += size;
            return std::fwrite(bytes, 1, size, state.file) == size ? 1 : 0;
        };
        remote_http_result result{};
        const bool received = remote_http_run(&request, &result) == 0 && result.status == 200;
        const bool written = std::fclose(file.release()) == 0;
        if (!received || !written) {
            error = stop ? "Download cancelado." : "Não foi possível baixar os dados do CNX.";
            if (result.error[0]) error += " " + std::string{result.error};
            else if (result.status && result.status != 200) error += " HTTP " + std::to_string(result.status);
            return false;
        }
        return true;
    };
}

std::vector<pe::ui::Translation> TranslationRows(const std::filesystem::path& mods,
                                                const std::vector<Eden::CNX::Entry>& entries) {
    std::vector<pe::ui::Translation> rows;
    rows.reserve(entries.size());
    for (const auto& entry : entries) {
        const auto installed = Eden::CNX::Detect(mods, entry);
        pe::ui::Translation row;
        row.name = entry.name;
        row.size = entry.size;
        row.error = entry.error;
        row.title_id = entry.title_id;
        row.installed = installed.installed;
        row.managed = installed.managed;
        rows.push_back(std::move(row));
    }
    return rows;
}

}

pe::ui::Translations EdenServices::translations() {
    std::unique_lock lock(cnx_lock_);
    if (!Eden::FilesystemAccess()) {
        cnx_.error = "As traduções precisam de acesso à pasta de jogos do console.";
        return cnx_;
    }
    if (cnx_enable_title_) {
        const auto title = cnx_enable_title_;
        cnx_enable_title_ = 0;
        const bool mod = Eden::SaveModEnabled(title, Eden::CNX::kModName, true);
        const bool game = Eden::SaveModsEnabled(title, true);
        if (!mod || !game) cnx_.error = "Tradução instalada. Não foi possível ativá-la; confira a tela Mods.";
    }
    const auto now = static_cast<std::int64_t>(std::time(nullptr));
    if (!cnx_.loading && !cnx_.busy && (!cnx_checked_ || now < cnx_checked_ || now - cnx_checked_ >= 86400)) {
        if (cnx_thread_.joinable()) cnx_thread_.join();
        cnx_checked_ = now;
        cnx_.loading = true;
        cnx_.error.clear();
        cnx_stop_ = false;
        const auto mods = Eden::AssetsPath("mods");
        const auto cache = Eden::ConfigFile("cnx/translations.json");
        try {
            cnx_thread_ = std::thread([this, mods, cache, now] {
                std::vector<Eden::CNX::Entry> entries;
                std::vector<pe::ui::Translation> rows;
                std::string error;
                bool cached = false;
                bool loaded = false;
                try {
                    loaded = Eden::CNX::LoadCatalog(cache, now, FetchTranslations(cnx_stop_), entries, cached, error);
                    if (loaded) rows = TranslationRows(mods, entries);
                } catch (const std::exception& exception) {
                    error = exception.what();
                    loaded = false;
                }
                const std::lock_guard done(cnx_lock_);
                if (loaded) {
                    cnx_entries_ = std::move(entries);
                    cnx_.list = std::move(rows);
                }
                cnx_.cached = cached;
                cnx_.error = std::move(error);
                cnx_.loading = false;
                ++cnx_.generation;
            });
        } catch (const std::exception& exception) {
            cnx_.loading = false;
            cnx_.error = exception.what();
            ++cnx_.generation;
        }
    }
    return cnx_;
}

bool EdenServices::StartTranslation(std::uint64_t title_id, bool remove) {
    std::unique_lock lock(cnx_lock_);
    if (!Eden::FilesystemAccess() || cnx_.loading || cnx_.busy) return false;
    const auto found = std::find_if(cnx_entries_.begin(), cnx_entries_.end(), [title_id](const auto& entry) {
        return entry.title_id == title_id && entry.error.empty();
    });
    if (found == cnx_entries_.end()) return false;
    if (cnx_thread_.joinable()) cnx_thread_.join();
    cnx_.busy = true;
    cnx_.active_title = title_id;
    cnx_.error.clear();
    cnx_stop_ = false;
    const auto mods = Eden::AssetsPath("mods");
    try {
        cnx_thread_ = std::thread([this, mods, entry = *found, entries = cnx_entries_, remove] {
            std::string error;
            bool success = false;
            std::vector<pe::ui::Translation> rows;
            try {
                success = remove ? Eden::CNX::Remove(mods, entry, error) :
                                   Eden::CNX::Install(mods, entry, FetchTranslations(cnx_stop_), error);
                rows = TranslationRows(mods, entries);
            } catch (const std::exception& exception) {
                error = exception.what();
            }
            const std::lock_guard done(cnx_lock_);
            if (!rows.empty()) cnx_.list = std::move(rows);
            if (success && !remove) cnx_enable_title_ = entry.title_id;
            cnx_.error = std::move(error);
            cnx_.busy = false;
            cnx_.active_title = 0;
            ++cnx_.generation;
        });
    } catch (const std::exception& exception) {
        cnx_.busy = false;
        cnx_.active_title = 0;
        cnx_.error = exception.what();
        ++cnx_.generation;
        return false;
    }
    return true;
}

bool EdenServices::install_translation(std::uint64_t title_id) { return StartTranslation(title_id, false); }
bool EdenServices::remove_translation(std::uint64_t title_id) { return StartTranslation(title_id, true); }

void EdenServices::StopTranslations() {
    cnx_stop_ = true;
    if (cnx_thread_.joinable()) cnx_thread_.join();
}
