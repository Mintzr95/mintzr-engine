#include "texture_cache.h"
#include <algorithm>

namespace mju::assets {
const Image* TextureCache::load(const std::string&path){
    if(path.empty())return nullptr;
    ++tick_;
    auto it=entries_.find(path);
    if(it!=entries_.end()){it->second.last_used=tick_;return &it->second.image;}
    Image image;
    if(!load_image_rgba(path,image)||!image.valid())return nullptr;
    const std::size_t bytes=image.pixels.size();
    auto [inserted,ok]=entries_.try_emplace(path);if(!ok)return nullptr;
    inserted->second.image=std::move(image);inserted->second.last_used=tick_;bytes_+=bytes;
    trim();
    auto now=entries_.find(path);
    return now==entries_.end()?nullptr:&now->second.image;
}
const Image* TextureCache::find(const std::string&path)const{auto it=entries_.find(path);return it==entries_.end()?nullptr:&it->second.image;}
void TextureCache::clear(){entries_.clear();bytes_=0;tick_=0;}
void TextureCache::trim(){
    while(bytes_>budget_bytes_&&!entries_.empty()){
        auto victim=entries_.begin();
        for(auto it=std::next(entries_.begin());it!=entries_.end();++it)if(it->second.last_used<victim->second.last_used)victim=it;
        bytes_-=victim->second.image.pixels.size();entries_.erase(victim);
    }
}
}
