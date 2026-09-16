#include "scene_json.h"
#include <algorithm>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <unordered_set>

namespace mju::io {
using json=nlohmann::json;
namespace {
json entity_to_json(const Entity&e){return {{"id",e.id},{"parent",e.parent},{"name",e.name},{"position",{e.transform.position.x,e.transform.position.y}},{"scale",{e.transform.scale.x,e.transform.scale.y}},{"rotation",e.transform.rotation},{"color",{e.sprite.color.r,e.sprite.color.g,e.sprite.color.b,e.sprite.color.a}},{"size",{e.sprite.size.x,e.sprite.size.y}},{"visible",e.sprite.visible},{"texture",e.sprite.texture},{"uv_min",{e.sprite.uv_min.x,e.sprite.uv_min.y}},{"uv_max",{e.sprite.uv_max.x,e.sprite.uv_max.y}},{"frame_width",e.sprite.frame_width},{"frame_height",e.sprite.frame_height},{"frame",e.sprite.frame},{"frame_count",e.sprite.frame_count},{"fps",e.sprite.fps},{"animation_time",e.sprite.animation_time},{"animation_loop",e.sprite.animation_loop},{"animation_playing",e.sprite.animation_playing},{"layer",e.layer},{"active",e.active},{"entity_visible",e.visible},{"locked",e.locked}};}
std::uint64_t fnv1a(const std::string&s){std::uint64_t h=1469598103934665603ull;for(unsigned char c:s){h^=c;h*=1099511628211ull;}return h;}
std::string canonical_without_checksum(const json&root){json copy=root;copy.erase("checksum");return copy.dump(-1);}
}
std::string scene_to_json(const Scene&scene,int indent){json root;root["format"]="mju-scene";root["version"]=3;root["next_id"]=scene.next_id();root["entities"]=json::array();for(const auto&e:scene.entities())root["entities"].push_back(entity_to_json(e));root["checksum"]=fnv1a(canonical_without_checksum(root));return root.dump(indent);}

bool scene_from_json(Scene&scene,const std::string&text){
    try{
        const json root=json::parse(text);
        if(root.value("format","")!="mju-scene")return false;
        const int version=root.value("version",1);if(version<1||version>3||!root.contains("entities")||!root["entities"].is_array())return false;
        if(root.contains("checksum")&&root["checksum"].is_number_unsigned())if(root["checksum"].get<std::uint64_t>()!=fnv1a(canonical_without_checksum(root)))return false;
        scene.clear();std::vector<EntityId>ids;std::vector<EntityId>parents;std::unordered_set<EntityId>seen;EntityId max_id=1;
        for(const auto&v:root["entities"]){
            const EntityId id=v.value("id",0u);if(!id||!seen.insert(id).second)return false;
            auto*e=scene.create_entity_with_id(v.value("name","Entity"),id,0);if(!e)return false;
            ids.push_back(id);parents.push_back(v.value("parent",0u));
            auto p=v.value("position",std::vector<float>{0,0}),s=v.value("scale",std::vector<float>{1,1}),c=v.value("color",std::vector<float>{1,1,1,1}),sz=v.value("size",std::vector<float>{96,96});
            if(p.size()>=2)e->transform.position={p[0],p[1]};if(s.size()>=2)e->transform.scale={s[0],s[1]};e->transform.rotation=v.value("rotation",0.0f);
            if(c.size()>=4)e->sprite.color={c[0],c[1],c[2],c[3]};if(sz.size()>=2)e->sprite.size={sz[0],sz[1]};e->sprite.visible=v.value("visible",true);e->sprite.texture=v.value("texture",std::string{});
            auto u0=v.value("uv_min",std::vector<float>{0,0}),u1=v.value("uv_max",std::vector<float>{1,1});if(u0.size()>=2)e->sprite.uv_min={u0[0],u0[1]};if(u1.size()>=2)e->sprite.uv_max={u1[0],u1[1]};
            e->sprite.frame_width=std::max(0,v.value("frame_width",0));e->sprite.frame_height=std::max(0,v.value("frame_height",0));e->sprite.frame=std::max(0,v.value("frame",0));e->sprite.frame_count=std::max(1,v.value("frame_count",1));e->sprite.fps=std::max(0.0f,v.value("fps",0.0f));e->sprite.animation_time=std::max(0.0f,v.value("animation_time",0.0f));e->sprite.animation_loop=v.value("animation_loop",true);e->sprite.animation_playing=v.value("animation_playing",false);
            e->layer=v.value("layer",0);e->active=v.value("active",true);e->visible=v.value("entity_visible",true);e->locked=v.value("locked",false);max_id=std::max(max_id,id==UINT32_MAX?UINT32_MAX:id+1);
        }
        // Validate and restore hierarchy only after all IDs exist, so forward parents are safe.
        for(std::size_t i=0;i<ids.size();++i){const EntityId p=parents[i];if(p&&p==ids[i])return false;if(p&&!scene.find(p))return false;if(!scene.set_parent(ids[i],p))return false;}
        scene.reset_id_counter(std::max<EntityId>(max_id,root.value("next_id",max_id)));return true;
    }catch(...){return false;}
}
bool save_scene_json(const Scene&scene,const std::string&path){std::ofstream out(path,std::ios::binary|std::ios::trunc);if(!out)return false;out<<scene_to_json(scene,2);out.flush();return out.good();}
bool load_scene_json(Scene&scene,const std::string&path){std::ifstream in(path,std::ios::binary);if(!in)return false;return scene_from_json(scene,std::string(std::istreambuf_iterator<char>(in),std::istreambuf_iterator<char>()));}
}
