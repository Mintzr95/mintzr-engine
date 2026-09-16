#pragma once
#include "image_loader.h"
#include <cstddef>
#include <string>
#include <unordered_map>
namespace mju::assets {
class TextureCache {
public:
    const Image* load(const std::string& path);
    const Image* find(const std::string& path) const;
    void clear();
    std::size_t size() const{return items_.size();}
private:
    std::unordered_map<std::string,Image> items_;
};
}
