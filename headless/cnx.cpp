#include "cnx.h"

#include "miniz.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cerrno>
#include <charconv>
#include <cctype>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <sys/stat.h>
#include <unistd.h>

#if defined(__PROSPERO__)
#include "native_directory.h"
#endif

namespace Eden::CNX {
namespace {
namespace fs = std::filesystem;
using Json = nlohmann::json;

constexpr std::size_t kCatalogLimit = 4u << 20;
constexpr std::int64_t kDay = 86400;
constexpr const char* kOwnedFile = ".cnx-owned";
constexpr std::string_view kOwnedValue = "CNX PT-BR 1\n";

std::string Lower(std::string_view text) {
    std::string result(text);
    for (char& c : result) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return result;
}

std::string TitleName(std::uint64_t title) {
    char text[17];
    std::snprintf(text, sizeof(text), "%016llX", static_cast<unsigned long long>(title));
    return text;
}

std::uint64_t Title(std::string_view text) {
    if (text.size() != 16) return 0;
    std::uint64_t value = 0;
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), value, 16);
    return parsed.ec == std::errc{} && parsed.ptr == text.data() + text.size() ? value : 0;
}

bool Relative(std::string_view text) {
    if (text.empty() || text.front() == '/' || text.find('\\') != text.npos || text.find(':') != text.npos)
        return false;
    for (const unsigned char c : text)
        if (c < 32 || c == 127) return false;
    for (;;) {
        const auto slash = text.find('/');
        const auto part = text.substr(0, slash);
        if (part.empty() || part == "." || part == "..") return false;
        if (slash == text.npos) return true;
        text.remove_prefix(slash + 1);
    }
}

std::uint64_t PathTitle(std::string_view path) {
    if (!Relative(path)) return 0;
    const auto prefix = path.substr(0, path.find('/'));
    if (prefix.size() == 15 && prefix.front() == '1') return Title("0" + std::string(prefix));
    return Title(prefix);
}

std::uint64_t ZipTitle(std::string_view link) {
    link = link.substr(0, link.find_first_of("?#"));
    const auto slash = link.rfind('/');
    if (slash != link.npos) link.remove_prefix(slash + 1);
    if (Lower(link).ends_with(".zip")) link.remove_suffix(4);
    std::uint64_t found = 0;
    for (std::size_t i = 0; i + 16 <= link.size(); ++i) {
        const auto hex = [](unsigned char c) { return std::isxdigit(c) != 0; };
        if ((i && hex(static_cast<unsigned char>(link[i - 1]))) ||
            (i + 16 < link.size() && hex(static_cast<unsigned char>(link[i + 16])))) continue;
        if (const auto title = Title(link.substr(i, 16))) {
            if (found && found != title) return 0;
            found = title;
        }
    }
    return found;
}

std::string Text(const Json& object, const char* name) {
    const auto value = object.find(name);
    return value != object.end() && value->is_string() ? value->get<std::string>() : std::string{};
}

bool ReleaseLink(std::string_view link) {
    constexpr std::string_view prefix = "https://github.com/AMSNX/Traducoes-BR-NX/releases/download/";
    if (!link.starts_with(prefix) || link.size() <= prefix.size()) return false;
    for (const unsigned char c : link)
        if (c <= 32 || c == 127 || c == '\\') return false;
    return true;
}

bool Directory(const fs::path& path) {
    const int descriptor = open(path.c_str(), O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
    if (descriptor < 0) return false;
    close(descriptor);
    return true;
}

bool SafeDirectories(const fs::path& path, bool create) {
    if (!path.is_absolute()) return false;
    fs::path current = path.root_path();
    for (const auto& component : path.relative_path()) {
        if (component == "." || component == "..") return false;
        current /= component;
        if (create && mkdir(current.c_str(), 0777) != 0 && errno != EEXIST) return false;
        if (!Directory(current)) return false;
    }
    return true;
}

std::vector<fs::path> Children(const fs::path& path) {
    std::vector<fs::path> paths;
    std::error_code error;
#if defined(__PROSPERO__)
    for (const auto& entry : Eden::ReadNativeDirectory(path, error)) paths.push_back(entry.path());
#else
    for (fs::directory_iterator it(path, error), end; !error && it != end; it.increment(error))
        paths.push_back(it->path());
#endif
    if (error) paths.clear();
    std::sort(paths.begin(), paths.end());
    return paths;
}

fs::path Child(const fs::path& parent, std::string_view name, bool directory) {
    const std::string wanted = Lower(name);
    for (const auto& path : Children(parent))
        if (Lower(path.filename().string()) == wanted && (!directory || Directory(path))) return path;
    return {};
}

fs::path TitleFolder(const fs::path& root, std::uint64_t title) {
    return title && SafeDirectories(root, false) ? Child(root, TitleName(title), true) : fs::path{};
}

bool SizedFile(const fs::path& root, std::string_view relative, std::uint64_t size) {
    if (!Relative(relative)) return false;
    fs::path path = root;
    for (;;) {
        const auto slash = relative.find('/');
        path = Child(path, relative.substr(0, slash), slash != relative.npos);
        if (path.empty()) return false;
        if (slash == relative.npos) break;
        relative.remove_prefix(slash + 1);
    }
    const int descriptor = open(path.c_str(), O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (descriptor < 0) return false;
    struct stat info {};
    const bool matches = fstat(descriptor, &info) == 0 && S_ISREG(info.st_mode) && info.st_size >= 0 &&
                         static_cast<std::uint64_t>(info.st_size) == size;
    close(descriptor);
    return matches;
}

bool Owned(const fs::path& folder) {
    const int descriptor = open((folder / kOwnedFile).c_str(), O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    if (descriptor < 0) return false;
    struct stat info {};
    char value[32]{};
    const bool owned = fstat(descriptor, &info) == 0 && S_ISREG(info.st_mode) &&
                       info.st_size == static_cast<off_t>(kOwnedValue.size()) &&
                       read(descriptor, value, sizeof(value)) == static_cast<ssize_t>(kOwnedValue.size()) &&
                       std::string_view(value, kOwnedValue.size()) == kOwnedValue;
    close(descriptor);
    return owned;
}

bool MarkOwned(const fs::path& folder) {
    const int descriptor = open((folder / kOwnedFile).c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0644);
    if (descriptor < 0) return false;
    const bool wrote = write(descriptor, kOwnedValue.data(), kOwnedValue.size()) ==
                       static_cast<ssize_t>(kOwnedValue.size());
    const bool closed = close(descriptor) == 0;
    return wrote && closed;
}

bool RemoveTree(const fs::path& path) {
    if (!Directory(path)) return unlink(path.c_str()) == 0 || errno == ENOENT;
    for (const auto& child : Children(path))
        if (!RemoveTree(child)) return false;
    return rmdir(path.c_str()) == 0;
}

struct Temporary {
    fs::path path;
    ~Temporary() { if (!path.empty()) RemoveTree(path); }
    bool Create(const fs::path& parent) {
        std::string pattern = (parent / ".cnx-XXXXXX").string();
        if (!mkdtemp(pattern.data())) return false;
        path = pattern;
        return true;
    }
};

bool Read(const fs::path& path, std::string& text) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) return false;
    text.clear();
    char buffer[8192];
    while (stream.read(buffer, sizeof(buffer)) || stream.gcount()) {
        if (text.size() + static_cast<std::size_t>(stream.gcount()) > kCatalogLimit) return false;
        text.append(buffer, static_cast<std::size_t>(stream.gcount()));
    }
    return stream.eof();
}

bool WriteCache(const fs::path& file, const Json& catalog, std::int64_t checked, std::int64_t updated) {
    if (!SafeDirectories(file.parent_path(), true)) return false;
    Temporary temporary;
    if (!temporary.Create(file.parent_path())) return false;
    const auto path = temporary.path / "cache.json";
    std::ofstream stream(path, std::ios::binary);
    stream << Json{{"checked_at", checked}, {"updated_at", updated}, {"catalog", catalog}}.dump();
    stream.close();
    if (!stream) return false;
    return rename(path.c_str(), file.c_str()) == 0;
}

class ZipReader {
public:
    explicit ZipReader(const fs::path& path) { opened_ = mz_zip_reader_init_file(&zip_, path.c_str(), 0) != 0; }
    ~ZipReader() { if (opened_) mz_zip_reader_end(&zip_); }
    bool Opened() const { return opened_; }
    mz_zip_archive* Get() { return &zip_; }
private:
    mz_zip_archive zip_{};
    bool opened_ = false;
};

struct ZipEntry {
    mz_uint index;
    std::string relative;
    bool directory;
};

bool ZipEntries(mz_zip_archive* zip, std::uint64_t title, std::vector<ZipEntry>& entries) {
    const auto count = mz_zip_reader_get_num_files(zip);
    if (!count || count > 100000) return false;
    std::set<std::string> names;
    std::map<std::string, std::string> components;
    std::set<std::string> regular_files;
    std::uint64_t bytes = 0;
    bool files = false;
    for (mz_uint index = 0; index < count; ++index) {
        mz_zip_archive_file_stat info{};
        if (!mz_zip_reader_file_stat(zip, index, &info) || !info.m_is_supported || info.m_is_encrypted)
            return false;
        const auto length = mz_zip_reader_get_filename(zip, index, nullptr, 0);
        if (length < 2 || length > 4096) return false;
        std::string name(length, '\0');
        if (mz_zip_reader_get_filename(zip, index, name.data(), length) != length) return false;
        name.resize(length - 1);
        const bool directory = name.back() == '/';
        if (directory != (info.m_is_directory != 0)) return false;
        if (directory) name.pop_back();
        if (!Relative(name) || Title(std::string_view(name).substr(0, name.find('/'))) != title) return false;
        const auto type = (info.m_external_attr >> 16) & 0170000u;
        if (type != 0 && type != (directory ? 0040000u : 0100000u)) return false;
        if (info.m_external_attr & 0x400u) return false;
        const auto slash = name.find('/');
        if (slash == name.npos) {
            if (!directory) return false;
            continue;
        }
        std::string relative = name.substr(slash + 1);
        if (Lower(relative) == kOwnedFile) return false;
        if (!names.insert(Lower(relative)).second) return false;
        for (std::size_t at = 0;;) {
            const auto end = relative.find('/', at);
            const auto prefix = relative.substr(0, end);
            const auto key = Lower(prefix);
            const auto known = components.find(key);
            if (known != components.end() && known->second != prefix) return false;
            components[key] = prefix;
            if (end == relative.npos) break;
            at = end + 1;
        }
        if (!directory) regular_files.insert(Lower(relative));
        if (info.m_uncomp_size > (32ull << 30) - bytes) return false;
        bytes += info.m_uncomp_size;
        entries.push_back({index, std::move(relative), directory});
        files |= !directory;
    }
    for (const auto& entry : entries)
        for (auto slash = entry.relative.find('/'); slash != entry.relative.npos; slash = entry.relative.find('/', slash + 1))
            if (regular_files.contains(Lower(std::string_view(entry.relative).substr(0, slash)))) return false;
    return files;
}

std::size_t Extract(void* user, mz_uint64 offset, const void* bytes, std::size_t size) {
    const int descriptor = *static_cast<int*>(user);
    std::size_t done = 0;
    while (done < size) {
        const auto wrote = pwrite(descriptor, static_cast<const char*>(bytes) + done, size - done,
                                  static_cast<off_t>(offset + done));
        if (wrote < 0 && errno == EINTR) continue;
        if (wrote <= 0) return done;
        done += static_cast<std::size_t>(wrote);
    }
    return done;
}

bool Unpack(mz_zip_archive* zip, const std::vector<ZipEntry>& entries, const fs::path& folder) {
    for (const auto& entry : entries) {
        const fs::path path = folder / entry.relative;
        if (!SafeDirectories(entry.directory ? path : path.parent_path(), true)) return false;
        if (entry.directory) continue;
        int descriptor = open(path.c_str(), O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW, 0644);
        if (descriptor < 0) return false;
        const bool ok = mz_zip_reader_extract_to_callback(zip, entry.index, Extract, &descriptor, 0) != 0;
        const bool closed = close(descriptor) == 0;
        if (!ok || !closed) return false;
    }
    return true;
}

}

std::vector<Entry> ParseCatalog(std::string_view text, std::string& error) {
    error.clear();
    std::vector<Entry> entries;
    if (text.size() > kCatalogLimit) {
        error = "A lista CNX excede o tamanho permitido.";
        return entries;
    }
    const Json document = Json::parse(text, nullptr, false);
    if (document.is_discarded() || !document.is_object() || !document.contains("translations") ||
        !document["translations"].is_object()) {
        error = "A lista CNX não contém traduções válidas.";
        return entries;
    }
    for (const auto& [name, value] : document["translations"].items()) {
        if (value.is_object() && value.contains("enabled") && value["enabled"].is_boolean() &&
            !value["enabled"].get<bool>()) continue;
        Entry entry;
        entry.name = name;
        if (!value.is_object()) {
            entry.error = "Metadados da tradução inválidos.";
            entries.push_back(std::move(entry));
            continue;
        }
        entry.size = Text(value, "size");
        entry.link = Text(value, "link");
        const auto installed = value.value("installed", Json::object());
        const std::string hash = installed.is_object() ? Text(installed, "hash_file") : std::string{};
        const auto uninstall = installed.is_object() ? installed.value("uninstall", Json::object()) : Json{};
        const auto from_hash = PathTitle(hash);
        const auto from_uninstall = uninstall.is_object() ? PathTitle(Text(uninstall, "path01")) : 0;
        const auto from_zip = ZipTitle(entry.link);
        entry.title_id = from_hash ? from_hash : from_uninstall ? from_uninstall : from_zip;
        if (from_hash && hash.find('/') != hash.npos) entry.hash_file = hash.substr(hash.find('/') + 1);
        const bool hash_size = installed.is_object() && installed.contains("hash_size") &&
                               (installed["hash_size"].is_number_unsigned() ||
                                (installed["hash_size"].is_number_integer() && installed["hash_size"].get<std::int64_t>() >= 0));
        if (hash_size) entry.hash_size = installed["hash_size"].get<std::uint64_t>();
        const bool enabled = value.contains("enabled") && value["enabled"].is_boolean() && value["enabled"].get<bool>();
        const bool valid_hash = !installed.is_object() || !installed.contains("hash_file") ||
                                installed["hash_file"].is_string();
        if (!enabled || !valid_hash || !entry.title_id || entry.name.empty() || entry.size.empty() || !ReleaseLink(entry.link) ||
            (!hash.empty() && (entry.hash_file.empty() || !hash_size)))
            entry.error = "Metadados da tradução incompletos ou inválidos.";
        for (const auto source : {from_hash, from_uninstall, from_zip})
            if (source && source != entry.title_id)
                entry.error = "Os Title IDs dos metadados CNX são inconsistentes.";
        entries.push_back(std::move(entry));
    }
    std::map<std::uint64_t, unsigned> titles;
    for (const auto& entry : entries)
        if (entry.error.empty()) ++titles[entry.title_id];
    for (auto& entry : entries)
        if (entry.error.empty() && titles[entry.title_id] > 1)
            entry.error = "O mesmo Title ID aparece em mais de uma tradução CNX.";
    if (entries.empty()) error = "A lista CNX está vazia.";
    else if (std::none_of(entries.begin(), entries.end(), [](const Entry& entry) { return entry.error.empty(); }))
        error = "A lista CNX não contém traduções utilizáveis.";
    return entries;
}

bool LoadCatalog(const fs::path& cache_file, std::int64_t now, const Fetch& fetch,
                 std::vector<Entry>& entries, bool& cached, std::string& error) {
    entries.clear();
    cached = false;
    error.clear();
    Json saved;
    std::int64_t checked = 0;
    std::int64_t updated = 0;
    std::string body;
    if (Read(cache_file, body)) {
        const Json envelope = Json::parse(body, nullptr, false);
        if (envelope.is_object() && envelope.contains("catalog")) {
            std::string ignored;
            entries = ParseCatalog(envelope["catalog"].dump(), ignored);
            if (ignored.empty()) {
                saved = envelope["catalog"];
                if (envelope.contains("checked_at") && envelope["checked_at"].is_number_integer())
                    checked = envelope["checked_at"].get<std::int64_t>();
                if (envelope.contains("updated_at") && envelope["updated_at"].is_number_integer())
                    updated = envelope["updated_at"].get<std::int64_t>();
                cached = true;
                if (checked > 0 && now >= checked && now - checked < kDay) return true;
            } else entries.clear();
        }
    }
    Temporary temporary;
    if (!SafeDirectories(cache_file.parent_path(), true) || !temporary.Create(cache_file.parent_path())) {
        error = "Não foi possível criar o cache CNX.";
        return cached;
    }
    const fs::path downloaded = temporary.path / "catalog.json";
    bool fetched = fetch && fetch(kCatalogUrl, downloaded, error);
    std::vector<Entry> fresh;
    Json document;
    if (fetched && Read(downloaded, body)) {
        fresh = ParseCatalog(body, error);
        fetched = error.empty();
        if (fetched) document = Json::parse(body);
    } else {
        fetched = false;
        if (error.empty()) error = "Não foi possível atualizar a lista CNX.";
    }
    if (!fetched) {
        if (cached) WriteCache(cache_file, saved, now, updated);
        return cached;
    }
    if (!WriteCache(cache_file, document, now, now)) {
        error = "Não foi possível salvar a lista CNX no cache.";
        return cached;
    }
    entries = std::move(fresh);
    cached = false;
    return true;
}

Installation Detect(const fs::path& mods_root, const Entry& entry) {
    Installation result;
    if (!entry.error.empty()) return result;
    const fs::path title = TitleFolder(mods_root, entry.title_id);
    if (title.empty()) return result;
    const fs::path managed = Child(title, kModName, true);
    const bool folder = !managed.empty();
    result.managed = folder && Owned(managed);
    if (entry.hash_file.empty()) {
        result.installed = folder;
        if (folder && !result.managed) result.manual_mod = managed.filename().string();
        return result;
    }
    if (result.managed && SizedFile(managed, entry.hash_file, entry.hash_size)) result.installed = true;
    for (const auto& mod : Children(title)) {
        if ((mod == managed && result.managed) || !Directory(mod) || !SizedFile(mod, entry.hash_file, entry.hash_size)) continue;
        result.installed = true;
        result.manual_mod = mod.filename().string();
        break;
    }
    return result;
}

bool Install(const fs::path& mods_root, const Entry& entry, const Fetch& fetch, std::string& error) {
    error.clear();
    if (!entry.error.empty() || !entry.title_id || !ReleaseLink(entry.link) ||
        (!entry.hash_file.empty() && !Relative(entry.hash_file))) {
        error = entry.error.empty() ? "Metadados da tradução inválidos." : entry.error;
        return false;
    }
    if (!SafeDirectories(mods_root, true)) {
        error = "Não foi possível acessar a pasta de mods.";
        return false;
    }
    fs::path title = Child(mods_root, TitleName(entry.title_id), false);
    if (title.empty()) title = mods_root / TitleName(entry.title_id);
    else if (!Directory(title)) {
        error = "A pasta do jogo não é um diretório válido.";
        return false;
    }
    if (!Child(title, kModName, false).empty()) {
        error = "A pasta PT-BR CNX já existe e será preservada.";
        return false;
    }
    const fs::path destination = title / kModName;
    errno = 0;
    const int existing = open(destination.c_str(), O_RDONLY | O_NOFOLLOW | O_NONBLOCK);
    const int failure = errno;
    if (existing >= 0) close(existing);
    if (existing >= 0 || failure != ENOENT) {
        error = "A pasta PT-BR CNX já existe e será preservada.";
        return false;
    }
    Temporary temporary;
    if (!temporary.Create(mods_root)) {
        error = "Não foi possível criar a pasta temporária.";
        return false;
    }
    const fs::path archive = temporary.path / "translation.zip";
    if (!fetch || !fetch(entry.link, archive, error)) {
        if (error.empty()) error = "Não foi possível baixar a tradução.";
        return false;
    }
    ZipReader reader(archive);
    std::vector<ZipEntry> files;
    if (!reader.Opened() || !ZipEntries(reader.Get(), entry.title_id, files)) {
        error = "O ZIP contém caminhos inválidos ou arquivos fora do Title ID esperado.";
        return false;
    }
    const fs::path content = temporary.path / "content";
    if (!Unpack(reader.Get(), files, content)) {
        error = "Não foi possível extrair a tradução.";
        return false;
    }
    if (!entry.hash_file.empty() && !SizedFile(content, entry.hash_file, entry.hash_size)) {
        error = "O arquivo da tradução não corresponde aos metadados CNX.";
        return false;
    }
    if (!MarkOwned(content) || !SafeDirectories(title, true) || rename(content.c_str(), destination.c_str()) != 0) {
        error = "Não foi possível mover a tradução para a pasta de mods.";
        return false;
    }
    return true;
}

bool Remove(const fs::path& mods_root, const Entry& entry, std::string& error) {
    error.clear();
    const fs::path title = TitleFolder(mods_root, entry.title_id);
    if (title.empty()) {
        error = "A tradução instalada pelo app não foi encontrada.";
        return false;
    }
    const fs::path folder = Child(title, kModName, true);
    if (folder.empty() || !Owned(folder)) {
        error = "Não foi possível remover a pasta PT-BR CNX.";
        return false;
    }
    for (const auto& child : Children(folder)) {
        if (child.filename() == kOwnedFile) continue;
        if (!RemoveTree(child)) {
            error = "Não foi possível remover todos os arquivos da tradução. Tente novamente.";
            return false;
        }
    }
    if (unlink((folder / kOwnedFile).c_str()) != 0) {
        error = "Não foi possível finalizar a remoção da tradução.";
        return false;
    }
    if (rmdir(folder.c_str()) != 0) {
        MarkOwned(folder);
        error = "Não foi possível remover a pasta da tradução. Tente novamente.";
        return false;
    }
    return true;
}

}
