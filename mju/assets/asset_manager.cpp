#include "asset_manager.h"
#include <filesystem>
#include <algorithm>
#include <cctype>

namespace mju::assets {
Type AssetManager::detect_type(const std::string& path){
    auto ext=std::filesystem::path(path).extension().string();
    std::transform(ext.begin(),ext.end(),ext.begin(),[](unsigned char c){return char(std::tolower(c));});
    if(ext==".png"||ext==".jpg"||ext==".jpeg"||ext==".webp") return Type::Texture;
    if(ext==".wav"||ext==".ogg"||ext==".mp3") return Type::Audio;
    if(ext==".ttf"||ext==".otf") return Type::Font;
    if(ext==".mju") return Type::Scene;
    if(ext==".lua"||ext==".mjus") return Type::Script;
    if(ext==".json"||ext==".txt"||ext==".mjudata") return Type::Data;
    return Type::Unknown;
}
bool AssetManager::scan(const std::string& root){
    clear();
    std::error_code ec;
    if(!std::filesystem::exists(root,ec)) return false;
    for(auto it=std::filesystem::recursive_directory_iterator(root,ec); !ec && it!=std::filesystem::recursive_directory_iterator(); it.increment(ec)){
        if(ec || !it->is_regular_file()) continue;
        auto rel=std::filesystem::relative(it->path(),root,ec).generic_string(); if(ec) continue;
        Asset a{rel,detect_type(rel)};
        if(a.type!=Type::Unknown){ index_[a.path]=assets_.size(); assets_.push_back(std::move(a)); }
    }
    std::sort(assets_.begin(),assets_.end(),[](const Asset&a,const Asset&b){return a.path<b.path;});
    index_.clear(); for(size_t i=0;i<assets_.size();++i) index_[assets_[i].path]=i;
    return true;
}
const Asset* AssetManager::find(const std::string& path) const { auto it=index_.find(path); return it==index_.end()?nullptr:&assets_[it->second]; }
void AssetManager::clear(){ assets_.clear(); index_.clear(); }
}
