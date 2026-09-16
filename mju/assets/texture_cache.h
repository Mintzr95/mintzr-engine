#pragma once
#include "image_loader.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace mju::assets {
class TextureCache {
public:
    const Image* load(const std::string&path);
    const Image* find(const std::string&path) const;
    void clear();
    void set_budget_bytes(std::size_t bytes){budget_bytes_=bytes;trim();}
    std::size_t budget_bytes()const{return budget_bytes_;}
    std::size_t bytes()const{return bytes_;}
    std::size_t size()const{return entries_.size();}
private:
    struct Entry { Image image; std::uint64_t last_used=0; };
    void trim();
    std::unordered_map<std::string,Entry> entries_;
    std::size_t budget_bytes_=64u*1024u*1024u;
    std::size_t bytes_=0;
    std::uint64_t tick_=0;
};
}
