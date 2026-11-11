#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace Eden::CNX {

inline constexpr const char* kModName = "PT-BR CNX";
inline constexpr const char* kCatalogUrl = "https://raw.githubusercontent.com/gamemoddesignbr/nx-links/master/nx-links-v204.json";

struct Entry {
    std::string name;
    std::string size;
    std::string link;
    std::string hash_file;
    std::string error;
    std::uint64_t title_id = 0;
    std::uint64_t hash_size = 0;
};

struct Installation {
    bool installed = false;
    bool managed = false;
    std::string manual_mod;
};

using Fetch = std::function<bool(const std::string&, const std::filesystem::path&, std::string&)>;

std::vector<Entry> ParseCatalog(std::string_view text, std::string& error);
bool LoadCatalog(const std::filesystem::path& cache_file, std::int64_t now, const Fetch& fetch,
                 std::vector<Entry>& entries, bool& cached, std::string& error);
Installation Detect(const std::filesystem::path& mods_root, const Entry& entry);
bool Install(const std::filesystem::path& mods_root, const Entry& entry, const Fetch& fetch, std::string& error);
bool Remove(const std::filesystem::path& mods_root, const Entry& entry, std::string& error);

}
