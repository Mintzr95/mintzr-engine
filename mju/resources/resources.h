#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

namespace mju::resources {

enum class Type { Unknown, Texture, Audio, Font, Scene, Script, Data };
struct Texture { std::string path; int width = 0, height = 0; bool hasAlpha = true; };
struct AudioClip { std::string path; bool streaming = false; float volume = 1; };

struct PackEntry {
    std::string path;
    std::uint64_t offset = 0;
    std::uint64_t size = 0;
};

class AssetPack {
public:
    bool build(const std::string& source_directory, const std::string& output_path);
    bool open(const std::string& pack_path);
    void close();
    bool is_open() const { return open_; }
    const std::vector<PackEntry>& entries() const { return entries_; }
    bool contains(const std::string& path) const;
    bool read(const std::string& path, std::vector<std::uint8_t>& out) const;
    std::uint32_t version() const { return version_; }

private:
    static constexpr char kMagic[8] = {'M', 'J', 'U', 'P', 'A', 'C', 'K', '1'};
    std::uint32_t version_ = 1;
    std::string pack_path_;
    std::vector<PackEntry> entries_;
    std::unordered_map<std::string, std::size_t> index_;
    bool open_ = false;
};

class ResourceCatalog {
public:
    void clear();
    bool register_resource(std::string path, Type type);
    Type type_of(const std::string& path) const;
    std::vector<std::string> list(Type type) const;
    bool import_directory(const std::string& root);
private:
    static Type classify(const std::filesystem::path& p);
    std::unordered_map<std::string, Type> items_;
};
}
