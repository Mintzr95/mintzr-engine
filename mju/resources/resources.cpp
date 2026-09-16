#include "resources.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <limits>

namespace mju::resources {

namespace {

bool write_u32(std::ofstream& out, std::uint32_t value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    return static_cast<bool>(out);
}

bool write_u64(std::ofstream& out, std::uint64_t value) {
    out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    return static_cast<bool>(out);
}

bool read_u32(std::ifstream& in, std::uint32_t& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(value));
    return static_cast<bool>(in);
}

bool read_u64(std::ifstream& in, std::uint64_t& value) {
    in.read(reinterpret_cast<char*>(&value), sizeof(value));
    return static_cast<bool>(in);
}

}

bool AssetPack::build(const std::string& source_directory, const std::string& output_path) {
    close();
    if (source_directory.empty() || output_path.empty()) return false;

    std::error_code ec;
    const auto root = std::filesystem::weakly_canonical(source_directory, ec);
    if (ec || !std::filesystem::is_directory(root, ec)) return false;

    std::vector<std::filesystem::path> files;
    for (std::filesystem::recursive_directory_iterator it(root, ec), end; it != end; it.increment(ec)) {
        if (ec) return false;
        if (it->is_regular_file(ec)) files.push_back(it->path());
    }
    std::sort(files.begin(), files.end());

    std::string temporary = output_path + ".tmp";
    std::ofstream out(temporary, std::ios::binary | std::ios::trunc);
    if (!out) return false;

    out.write(kMagic, sizeof(kMagic));
    if (!write_u32(out, version_) || !write_u32(out, static_cast<std::uint32_t>(files.size()))) return false;

    std::vector<PackEntry> entries;
    entries.reserve(files.size());

    for (const auto& file : files) {
        std::ifstream in(file, std::ios::binary);
        if (!in) return false;
        const auto bytes = std::vector<std::uint8_t>(
            std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
        const auto relative = std::filesystem::relative(file, root, ec).generic_string();
        if (ec || relative.empty()) return false;
        if (relative.size() > std::numeric_limits<std::uint16_t>::max()) return false;
        if (static_cast<std::uint64_t>(bytes.size()) > std::numeric_limits<std::uint64_t>::max()) return false;

        const std::uint16_t path_length = static_cast<std::uint16_t>(relative.size());
        const auto offset = static_cast<std::uint64_t>(out.tellp());
        out.write(reinterpret_cast<const char*>(&path_length), sizeof(path_length));
        out.write(relative.data(), static_cast<std::streamsize>(relative.size()));
        if (!write_u64(out, static_cast<std::uint64_t>(bytes.size()))) return false;
        if (!bytes.empty()) {
            out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        }
        if (!out) return false;
        entries.push_back({relative, offset, static_cast<std::uint64_t>(bytes.size())});
    }

    out.flush();
    if (!out) return false;
    out.close();
    std::remove(output_path.c_str());
    if (std::rename(temporary.c_str(), output_path.c_str()) != 0) return false;
    return open(output_path);
}

bool AssetPack::open(const std::string& pack_path) {
    close();
    std::ifstream in(pack_path, std::ios::binary);
    if (!in) return false;

    char magic[sizeof(kMagic)]{};
    in.read(magic, sizeof(magic));
    if (!in || !std::equal(std::begin(kMagic), std::end(kMagic), std::begin(magic))) return false;

    std::uint32_t count = 0;
    if (!read_u32(in, version_) || version_ != 1 || !read_u32(in, count)) return false;
    if (count > 1000000u) return false;

    entries_.clear();
    index_.clear();
    entries_.reserve(count);
    index_.reserve(count);

    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint16_t path_length = 0;
        if (!in.read(reinterpret_cast<char*>(&path_length), sizeof(path_length))) return false;
        if (path_length == 0) return false;
        std::string relative(path_length, '\0');
        in.read(relative.data(), static_cast<std::streamsize>(relative.size()));
        if (!in) return false;

        std::uint64_t size = 0;
        const std::uint64_t data_position = static_cast<std::uint64_t>(in.tellg()) + sizeof(std::uint64_t);
        if (!read_u64(in, size)) return false;
        if (!index_.emplace(relative, entries_.size()).second) return false;

        entries_.push_back({relative, data_position, size});
        in.seekg(static_cast<std::streamoff>(size), std::ios::cur);
        if (!in) return false;
    }

    pack_path_ = pack_path;
    open_ = true;
    return true;
}

void AssetPack::close() {
    open_ = false;
    pack_path_.clear();
    entries_.clear();
    index_.clear();
    version_ = 1;
}

bool AssetPack::contains(const std::string& path) const {
    return index_.find(path) != index_.end();
}

bool AssetPack::read(const std::string& path, std::vector<std::uint8_t>& out) const {
    out.clear();
    const auto it = index_.find(path);
    if (!open_ || it == index_.end()) return false;
    const auto& entry = entries_[it->second];
    if (entry.size > static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) return false;

    std::ifstream in(pack_path_, std::ios::binary);
    if (!in) return false;
    in.seekg(static_cast<std::streamoff>(entry.offset), std::ios::beg);
    if (!in) return false;
    out.resize(static_cast<std::size_t>(entry.size));
    if (!out.empty()) in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(out.size()));
    return static_cast<bool>(in) || out.empty();
}

void ResourceCatalog::clear() {
    items_.clear();
}

bool ResourceCatalog::register_resource(std::string path, Type type) {
    if (path.empty()) return false;
    items_[std::move(path)] = type;
    return true;
}

Type ResourceCatalog::type_of(const std::string& path) const {
    auto it = items_.find(path);
    return it == items_.end() ? Type::Unknown : it->second;
}

std::vector<std::string> ResourceCatalog::list(Type type) const {
    std::vector<std::string> result;
    for (const auto& [path, value] : items_) {
        if (value == type) result.push_back(path);
    }
    std::sort(result.begin(), result.end());
    return result;
}

Type ResourceCatalog::classify(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    for (char& value : extension) value = static_cast<char>(std::tolower(static_cast<unsigned char>(value)));
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".webp") return Type::Texture;
    if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") return Type::Audio;
    if (extension == ".ttf" || extension == ".otf") return Type::Font;
    if (extension == ".mju" || extension == ".mjuscene") return Type::Scene;
    if (extension == ".lua" || extension == ".mjua" || extension == ".cpp" || extension == ".h") return Type::Script;
    if (extension == ".json" || extension == ".txt" || extension == ".csv" || extension == ".mjutile") return Type::Data;
    return Type::Unknown;
}

bool ResourceCatalog::import_directory(const std::string& root) {
    std::error_code ec;
    const auto base = std::filesystem::path(root);
    if (!std::filesystem::exists(base, ec)) return false;
    for (auto it = std::filesystem::recursive_directory_iterator(base, ec), end; it != end; it.increment(ec)) {
        if (ec) return false;
        if (!it->is_regular_file(ec)) continue;
        const auto type = classify(it->path());
        if (type != Type::Unknown) register_resource(it->path().generic_string(), type);
    }
    return true;
}
}
