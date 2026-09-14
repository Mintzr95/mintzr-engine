#include "resources.h"
#include <algorithm>
#include <cctype>
namespace mju::resources {
void ResourceCatalog::clear(){items_.clear();}
bool ResourceCatalog::register_resource(std::string p,Type t){if(p.empty())return false;items_[std::move(p)]=t;return true;}
Type ResourceCatalog::type_of(const std::string&p)const{auto it=items_.find(p);return it==items_.end()?Type::Unknown:it->second;}
std::vector<std::string> ResourceCatalog::list(Type t)const{std::vector<std::string> r;for(auto&[p,v]:items_)if(v==t)r.push_back(p);std::sort(r.begin(),r.end());return r;}
Type ResourceCatalog::classify(const std::filesystem::path&p){std::string e=p.extension().string();for(char&c:e)c=char(std::tolower(static_cast<unsigned char>(c)));if(e==".png"||e==".jpg"||e==".jpeg"||e==".webp")return Type::Texture;if(e==".wav"||e==".ogg"||e==".mp3")return Type::Audio;if(e==".ttf"||e==".otf")return Type::Font;if(e==".mju")return Type::Scene;if(e==".lua"||e==".mjua"||e==".cpp"||e==".h")return Type::Script;if(e==".json"||e==".txt"||e==".csv"||e==".mjutile")return Type::Data;return Type::Unknown;}
bool ResourceCatalog::import_directory(const std::string& root){std::error_code ec;auto base=std::filesystem::path(root);if(!std::filesystem::exists(base,ec))return false;for(auto it=std::filesystem::recursive_directory_iterator(base,ec);it!=std::filesystem::recursive_directory_iterator();it.increment(ec)){if(ec)break;if(!it->is_regular_file(ec))continue;auto t=classify(it->path());if(t!=Type::Unknown)register_resource(it->path().generic_string(),t);}return true;}
}
