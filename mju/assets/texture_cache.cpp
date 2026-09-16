#include "texture_cache.h"
namespace mju::assets {
const Image* TextureCache::load(const std::string& path){
    if(path.empty())return nullptr;
    auto it=items_.find(path); if(it!=items_.end())return &it->second;
    Image image; if(!load_image_rgba(path,image))return nullptr;
    auto [inserted,ok]=items_.emplace(path,std::move(image)); return ok?&inserted->second:nullptr;
}
const Image* TextureCache::find(const std::string& path) const{auto it=items_.find(path);return it==items_.end()?nullptr:&it->second;}
void TextureCache::clear(){items_.clear();}
}
