#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace mju::assets {

enum class Type { Unknown, Texture, Audio, Font, Scene, Script, Data };
struct Asset { std::string path; Type type=Type::Unknown; };

class AssetManager {
public:
    bool scan(const std::string& root);
    const Asset* find(const std::string& path) const;
    const std::vector<Asset>& all() const { return assets_; }
    void clear();
private:
    static Type detect_type(const std::string& path);
    std::vector<Asset> assets_;
    std::unordered_map<std::string, std::size_t> index_;
};
}
