#include "cnx.h"

#include "miniz.h"
#include <nlohmann/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace {
namespace fs = std::filesystem;
using Json = nlohmann::json;
using namespace Eden::CNX;

int checks = 0;

void Require(bool ok, const char* message) {
    ++checks;
    if (!ok) {
        std::fprintf(stderr, "CNX FAIL: %s\n", message);
        std::exit(1);
    }
}

void Write(const fs::path& path, const std::string& text) {
    fs::create_directories(path.parent_path());
    std::ofstream(path, std::ios::binary) << text;
}

Json Translation(const std::string& title = "0100ABCD00001000") {
    return {{"enabled", true}, {"size", "53.7 MB"},
            {"link", "https://github.com/AMSNX/Traducoes-BR-NX/releases/download/test/" + title + ".zip"},
            {"installed", {{"hash_file", title + "/romfs/lang/en.bin"}, {"hash_size", 4},
                           {"uninstall", {{"path01", title + "/romfs"}}}}}};
}

Entry ParseOne(Json value) {
    std::string error;
    const auto entries = ParseCatalog(Json{{"translations", {{"Game", value}}}}.dump(), error);
    Require(entries.size() == 1, "parse one catalog entry");
    return entries.front();
}

void Parsing() {
    auto entry = ParseOne(Translation());
    Require(entry.error.empty() && entry.size == "53.7 MB", "size is text");
    Require(entry.title_id == 0x0100ABCD00001000ull && entry.hash_file == "romfs/lang/en.bin" &&
            entry.hash_size == 4, "hash-file title and relative path");
    auto value = Translation();
    value["installed"].erase("hash_file");
    value["installed"].erase("hash_size");
    value["installed"]["uninstall"]["path01"] = "0100abcd00001000";
    entry = ParseOne(value);
    Require(entry.error.empty() && entry.hash_file.empty() && entry.title_id == 0x0100ABCD00001000ull,
            "Dark Devotion shape uses uninstall title without hash");
    value.erase("installed");
    entry = ParseOne(value);
    Require(entry.error.empty() && entry.title_id == 0x0100ABCD00001000ull, "ZIP filename title fallback");
    value["link"] = "https://github.com/AMSNX/Traducoes-BR-NX/releases/download/test/friendly.zip";
    value["installed"] = {{"hash_file", "0100abcd00001000/romfs/en.bin"}, {"hash_size", 4}};
    entry = ParseOne(value);
    Require(entry.error.empty() && entry.title_id == 0x0100ABCD00001000ull, "hash title without filename title");
    value["installed"]["uninstall"]["path01"] = "0100FFFF00002000/romfs";
    entry = ParseOne(value);
    Require(entry.title_id == 0x0100ABCD00001000ull && !entry.error.empty(), "hash source priority and conflict");
    value = Translation();
    value["installed"].erase("hash_file");
    value["installed"]["uninstall"]["path01"] = "0100FFFF00002000/romfs";
    entry = ParseOne(value);
    Require(entry.title_id == 0x0100FFFF00002000ull && !entry.error.empty(), "uninstall source priority and conflict");
    entry = ParseOne(Translation("0100414d32524e00"));
    Require(entry.error.empty() && entry.title_id == 0x0100414D32524E00ull, "preserve homebrew title suffix");
    value = Translation();
    value["installed"]["hash_file"] = "100ABCD00001000/romfs/lang/en.bin";
    value["installed"]["uninstall"]["path01"] = "100ABCD00001000/romfs";
    entry = ParseOne(value);
    Require(entry.error.empty() && entry.title_id == 0x0100ABCD00001000ull &&
            entry.hash_file == "romfs/lang/en.bin", "metadata without leading hexadecimal zero");
    value["installed"]["hash_size"] = 47.129;
    Require(!ParseOne(value).error.empty(), "fractional hash size rejected");
    value = Translation();
    value["installed"]["hash_file"] = "0100ABCD00001000/romfs/../../escape";
    Require(!ParseOne(value).error.empty(), "unsafe metadata path rejected");
    value = Translation();
    value["installed"].erase("hash_size");
    Require(!ParseOne(value).error.empty(), "missing hash size rejected");
    value = Translation();
    value["size"] = 537;
    Require(!ParseOne(value).error.empty(), "numeric size rejected");
    value = Translation();
    value["link"] = "http://github.com/AMSNX/Traducoes-BR-NX/releases/download/test/file.zip";
    Require(!ParseOne(value).error.empty(), "non-HTTPS release rejected");
    std::string error;
    const auto entries = ParseCatalog(Json{{"translations", {{"Valid", Translation()},
        {"Disabled", {{"enabled", false}}}}}, {"modifications", {{"Not a translation", Translation()}}}}.dump(), error);
    Require(error.empty() && entries.size() == 1 && entries[0].name == "Valid", "only enabled translations");
    Require(ParseCatalog("[]", error).empty() && !error.empty(), "invalid top-level shape");
    Require(ParseCatalog("not JSON", error).empty() && !error.empty(), "malformed JSON");
    const auto duplicates = ParseCatalog(Json{{"translations", {{"First", Translation()},
        {"Second", Translation()}}}}.dump(), error);
    Require(duplicates.size() == 2 && !duplicates[0].error.empty() && !duplicates[1].error.empty(),
            "duplicate valid title IDs are ambiguous");
    auto wrong = Translation();
    wrong["link"] = "https://github.com/AMSNX/Traducoes-BR-NX/releases/download/test/0100FFFF00002000.zip";
    const auto distinct = ParseCatalog(Json{{"translations", {{"First", Translation()},
        {"Second", wrong}}}}.dump(), error);
    Require(error.empty() && distinct.size() == 2 && distinct[0].error.empty() && !distinct[1].error.empty(),
            "invalid conflicting entry does not block valid entry");
}

void Detection(const fs::path& base) {
    auto entry = ParseOne(Translation());
    const fs::path root = base / "detection" / "mods";
    Require(!Detect(root, entry).installed, "missing mods folder");
    Write(root / "0100abcd00001000" / "ptbr" / "ROMFS" / "LANG" / "en.bin", "data");
    auto state = Detect(root, entry);
    Require(state.installed && !state.managed && state.manual_mod == "ptbr", "manual translation and folder case");
    Write(root / "0100abcd00001000" / "ptbr" / "ROMFS" / "LANG" / "en.bin", "bad");
    Require(!Detect(root, entry).installed, "wrong hash size is not installed");
    const auto managed = root / "0100abcd00001000" / kModName;
    Write(managed / "romfs/lang/en.bin", "data");
    state = Detect(root, entry);
    Require(state.installed && !state.managed && state.manual_mod == kModName,
            "manual mod with reserved name is not owned");
    std::string error;
    Require(!Remove(root, entry, error) && fs::exists(managed / "romfs/lang/en.bin"),
            "manual mod with reserved name is never removed");
    const auto lower_managed = root / "0100abcd00001000/pt-br cnx";
    fs::rename(managed, lower_managed);
    Require(!Install(root, entry, [](const auto&, const auto&, auto&) { return false; }, error) &&
            fs::exists(lower_managed / "romfs/lang/en.bin"), "manual reserved folder case preserved");
    fs::rename(lower_managed, managed);
    Write(managed / ".cnx-owned", "CNX PT-BR 1\n");
    state = Detect(root, entry);
    Require(state.installed && state.managed, "managed translation detected");
    entry.hash_file.clear();
    Require(Detect(root, entry).installed, "no-hash entry uses managed folder");
    fs::remove_all(managed);
    Require(!Detect(root, entry).installed, "no-hash ignores unrelated manual folder");
    entry = ParseOne(Translation());
    const auto external = base / "external";
    Write(external / "romfs/lang/en.bin", "data");
    fs::create_directory_symlink(external, root / "0100abcd00001000" / "linked");
    Require(!Detect(root, entry).installed, "symlink mod is not followed");
    fs::create_directory_symlink(external, managed);
    Require(!Remove(root, entry, error) && fs::exists(external / "romfs/lang/en.bin"), "managed symlink removal refused");
    fs::remove(managed);
    fs::remove(root / "0100abcd00001000" / "linked");
}

void Zip(const fs::path& path, const std::vector<std::pair<std::string, std::string>>& files,
         bool symlink = false) {
    mz_zip_archive archive{};
    Require(mz_zip_writer_init_file(&archive, path.c_str(), 0) != 0, "create fixture ZIP");
    std::vector<std::pair<std::string, std::string>> replacements;
    for (const auto& [name, data] : files) {
        std::string accepted = name;
        if (!accepted.empty() && accepted.front() == '/') {
            accepted.front() = 'x';
            replacements.emplace_back(accepted, name);
        }
        Require(mz_zip_writer_add_mem(&archive, accepted.c_str(), data.data(), data.size(), MZ_DEFAULT_COMPRESSION) != 0,
                "write fixture ZIP entry");
    }
    Require(mz_zip_writer_finalize_archive(&archive) != 0, "finalize fixture ZIP");
    Require(mz_zip_writer_end(&archive) != 0, "close fixture ZIP");
    if (symlink || !replacements.empty()) {
        std::fstream stream(path, std::ios::binary | std::ios::in | std::ios::out);
        std::string data{std::istreambuf_iterator<char>(stream), {}};
        for (const auto& [accepted, original] : replacements)
            for (std::size_t at = 0; (at = data.find(accepted, at)) != data.npos; at += original.size())
                data.replace(at, accepted.size(), original);
        if (symlink) {
            const auto offset = data.find(std::string("PK\1\2", 4));
            Require(offset != data.npos, "fixture ZIP central directory");
            data[offset + 5] = 3;
            const std::uint32_t attributes = 0120777u << 16;
            for (unsigned i = 0; i < 4; ++i) data[offset + 38 + i] = static_cast<char>(attributes >> (i * 8));
        }
        stream.seekp(0);
        stream.write(data.data(), static_cast<std::streamsize>(data.size()));
    }
}

Fetch Copy(const fs::path& source) {
    return [source](const std::string&, const fs::path& destination, std::string&) {
        std::error_code error;
        return fs::copy_file(source, destination, error);
    };
}

bool NoTemporary(const fs::path& root) {
    for (const auto& child : fs::directory_iterator(root))
        if (child.path().filename().string().starts_with(".cnx-")) return false;
    return true;
}

void Installing(const fs::path& base) {
    const auto entry = ParseOne(Translation());
    const auto archive = base / "fixture.zip";
    Zip(archive, {{"0100abcd00001000/romfs/lang/en.bin", "data"}});
    std::string error;
    for (const auto& path : {base / "data/eglantineps5/mods", base / "mnt/ext1/ROMs/Switch/mods"}) {
        Require(Install(path, entry, Copy(archive), error), "first installation and alternate games root");
        Require(fs::exists(path / "0100ABCD00001000/PT-BR CNX/romfs/lang/en.bin"), "ZIP title root stripped");
        Require(Detect(path, entry).installed, "installed files match metadata");
        const auto managed = path / "0100ABCD00001000/pt-br cnx";
        fs::rename(path / "0100ABCD00001000/PT-BR CNX", managed);
        Require(Detect(path, entry).managed, "managed folder detected after case-only rename");
        Require(!Install(path, entry, Copy(archive), error), "existing mod is never overwritten");
        if (geteuid() != 0) {
            const auto locked = managed / "locked";
            Write(locked / "data.bin", "preserve ownership until complete removal");
            Require(chmod(locked.c_str(), 0) == 0, "restrict directory for partial removal test");
            Require(!Remove(path, entry, error) && Detect(path, entry).managed,
                    "failed partial removal preserves ownership for retry");
            Require(chmod(locked.c_str(), 0700) == 0, "restore removable directory");
        }
        Write(path / "0100ABCD00001000/ptbr/romfs/lang/en.bin", "data");
        Require(Remove(path, entry, error), "remove managed mod");
        Require(!fs::exists(managed) &&
                fs::exists(path / "0100ABCD00001000/ptbr/romfs/lang/en.bin"), "manual translation preserved");
        Require(Detect(path, entry).installed, "manual state survives removal");
        Require(NoTemporary(path), "successful temporary cleanup");
    }
    const auto root = base / "unsafe/mods";
    const std::vector<std::string> unsafe = {
        "../escape", "/0100ABCD00001000/romfs/evil", "0100ABCD00001000/../escape",
        "0100ABCD00001000/romfs/../../escape", "0100ABCD00001000\\romfs\\evil",
        "0100ABCD00001000/romfs/C:evil", "0100FFFF00002000/romfs/evil",
        "readme.txt", "0100ABCD00001000/./romfs/evil", "0100ABCD00001000//romfs/evil"
    };
    for (const auto& name : unsafe) {
        Zip(archive, {{"0100ABCD00001000/romfs/lang/en.bin", "data"}, {name, "evil"}});
        Require(!Install(root, entry, Copy(archive), error), "unsafe archive refused before extraction");
        Require(!fs::exists(root / "0100ABCD00001000/PT-BR CNX"), "unsafe archive left no managed mod");
        Require(NoTemporary(root), "failed temporary cleanup");
    }
    Zip(archive, {{"0100ABCD00001000/romfs/lang/en.bin", "data"}}, true);
    Require(!Install(root, entry, Copy(archive), error), "ZIP symlink refused");
    Zip(archive, {{"0100ABCD00001000/romfs/lang/en.bin", "data"},
                  {"0100ABCD00001000/ROMFS/lang/EN.BIN", "evil"}});
    Require(!Install(root, entry, Copy(archive), error), "case-colliding ZIP entries refused");
    Zip(archive, {{"0100ABCD00001000/romfs/lang/en.bin", "data"},
                  {"0100ABCD00001000/ROMFS/other.bin", "evil"}});
    Require(!Install(root, entry, Copy(archive), error), "case-colliding ZIP parent directories refused");
    Zip(archive, {{"0100ABCD00001000/romfs", "evil"},
                  {"0100ABCD00001000/romfs/lang/en.bin", "data"}});
    Require(!Install(root, entry, Copy(archive), error), "ZIP file cannot also be a parent directory");
    Zip(archive, {{"0100ABCD00001000/romfs/lang/en.bin", "wrong"}});
    Require(!Install(root, entry, Copy(archive), error), "ZIP hash size mismatch refused");
    Require(NoTemporary(root), "validation failure cleanup");
    Require(!Install(root, entry, [](const auto&, const auto& destination, std::string& failure) {
        Write(destination, "partial download"); failure = "offline"; return false;
    }, error) && NoTemporary(root), "partial failed download cleanup");
    auto conflict = entry;
    conflict.error = "IDs inconsistentes";
    bool requested = false;
    Require(!Install(root, conflict, [&](const auto&, const auto&, auto&) { requested = true; return false; }, error)
            && !requested, "inconsistent metadata cannot download");
}

void Caching(const fs::path& base) {
    const auto cache = base / "cache/cnx.json";
    const auto good = Json{{"translations", {{"Game", Translation()}}}}.dump();
    std::vector<Entry> entries;
    bool cached = false;
    std::string error;
    int requests = 0;
    const Fetch online = [&](const auto& url, const auto& destination, auto&) {
        ++requests;
        Require(url == kCatalogUrl, "canonical CNX URL");
        Write(destination, good);
        return true;
    };
    Require(LoadCatalog(cache, 100000, online, entries, cached, error) && !cached && entries.size() == 1,
            "initial catalog saved");
    Require(LoadCatalog(cache, 100100, online, entries, cached, error) && cached && requests == 1,
            "same-day cache avoids network");
    Require(LoadCatalog(cache, 186400, online, entries, cached, error) && !cached && requests == 2,
            "catalog updates after 24 hours");
    const Fetch offline = [&](const auto&, const auto&, auto& failure) {
        ++requests; failure = "offline"; return false;
    };
    Require(LoadCatalog(cache, 272800, offline, entries, cached, error) && cached && !error.empty() &&
            entries.size() == 1 && entries[0].name == "Game", "offline uses last saved catalog");
    Require(LoadCatalog(cache, 272801, offline, entries, cached, error) && cached && requests == 3,
            "failed daily update is not repeated");
    const Fetch invalid = [&](const auto&, const auto& destination, auto&) {
        Write(destination, "{broken JSON"); return true;
    };
    Require(LoadCatalog(cache, 359200, invalid, entries, cached, error) && cached && !error.empty(),
            "bad response preserves valid cache");
    Require(LoadCatalog(cache, 359201, offline, entries, cached, error) && entries.size() == 1,
            "valid cache survives restart after bad response");
    const Fetch unusable = [&](const auto&, const auto& destination, auto&) {
        Write(destination, Json{{"translations", {{"Broken", {{"size", "53 MB"}}}}}}.dump()); return true;
    };
    Require(LoadCatalog(cache, 445600, unusable, entries, cached, error) && cached && !error.empty() &&
            entries.size() == 1 && entries.front().name == "Game", "unusable entries never replace valid cache");
    Require(!LoadCatalog(base / "no-cache/cnx.json", 100000, offline, entries, cached, error) &&
            entries.empty() && !error.empty(), "offline without saved catalog reports error");
}

}

int main(int argc, char** argv) {
    std::string temporary = "/private/tmp/cnx-check-XXXXXX";
#if !defined(__APPLE__)
    temporary = "/tmp/cnx-check-XXXXXX";
#endif
    Require(mkdtemp(temporary.data()) != nullptr, "create test directory");
    const fs::path base = temporary;
    Parsing();
    Detection(base);
    Installing(base);
    Caching(base);
    if (argc > 1) {
        std::ifstream stream(argv[1], std::ios::binary);
        const std::string body{std::istreambuf_iterator<char>(stream), {}};
        std::string error;
        const auto entries = ParseCatalog(body, error);
        Require(error.empty() && entries.size() == 128, "live catalog has 128 enabled entries");
        for (const auto& entry : entries)
            if (!entry.error.empty()) std::printf("CNX blocked: %s: %s\n", entry.name.c_str(), entry.error.c_str());
    }
    fs::remove_all(base);
    std::printf("CNX: %d checks passed\n", checks);
}
