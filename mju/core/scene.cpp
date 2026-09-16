#include "scene.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_set>

namespace mju {
namespace { constexpr float kDegToRad=3.14159265358979323846f/180.0f;
Vec2 rotate_vec(Vec2 v,float degrees){const float r=degrees*kDegToRad,c=std::cos(r),s=std::sin(r);return {v.x*c-v.y*s,v.x*s+v.y*c};} }
Scene::Scene(std::size_t max_entities):max_entities_(std::max<std::size_t>(1,max_entities)){}
Entity& Scene::create_entity(std::string name,EntityId parent){if(full())throw std::length_error("MJU scene entity limit reached");if(parent&&!find(parent))parent=0;Entity e{};e.id=next_id_++;if(next_id_==0)next_id_=1;e.parent=parent;e.name=std::move(name);entities_.push_back(std::move(e));Entity*ptr=&entities_.back();index_[ptr->id]=ptr;return *ptr;}
Entity* Scene::create_entity_with_id(std::string name,EntityId id,EntityId parent){if(!id||full()||find(id))return nullptr;if(parent&&!find(parent))return nullptr;Entity e{};e.id=id;e.parent=parent;e.name=std::move(name);entities_.push_back(std::move(e));Entity*ptr=&entities_.back();index_[id]=ptr;next_id_=std::max(next_id_,id+1);if(next_id_==0)next_id_=1;return ptr;}
Entity* Scene::find(EntityId id){auto it=index_.find(id);return it==index_.end()?nullptr:it->second;} const Entity* Scene::find(EntityId id)const{auto it=index_.find(id);return it==index_.end()?nullptr:it->second;}
std::vector<EntityId> Scene::children_of(EntityId parent)const{std::vector<EntityId>out;for(const auto&e:entities_)if(e.parent==parent)out.push_back(e.id);return out;}
bool Scene::would_create_cycle(EntityId child,EntityId parent)const{if(!parent)return false;EntityId cursor=parent;std::unordered_set<EntityId>seen;while(cursor){if(cursor==child)return true;if(!seen.insert(cursor).second)return true;const auto*e=find(cursor);if(!e)break;cursor=e->parent;}return false;}
bool Scene::set_parent(EntityId child,EntityId parent){auto*e=find(child);if(!e)return false;if(parent&&!find(parent))return false;if(would_create_cycle(child,parent))return false;e->parent=parent;return true;}
Transform2D Scene::combine(const Transform2D&p,const Transform2D&l){Transform2D out;out.scale={p.scale.x*l.scale.x,p.scale.y*l.scale.y};out.rotation=p.rotation+l.rotation;const Vec2 ls{l.position.x*p.scale.x,l.position.y*p.scale.y};out.position=p.position+rotate_vec(ls,p.rotation);return out;}
Transform2D Scene::world_transform(EntityId id)const{const Entity*e=find(id);if(!e)return{};Transform2D result=e->transform;EntityId cursor=e->parent;std::unordered_set<EntityId>seen;while(cursor){if(!seen.insert(cursor).second)break;const Entity*p=find(cursor);if(!p)break;result=combine(p->transform,result);cursor=p->parent;}return result;}
bool Scene::destroy_entity(EntityId id,bool destroy_children){if(!find(id))return false;std::unordered_set<EntityId>doomed{ id };if(destroy_children){bool changed=true;while(changed){changed=false;for(const auto&e:entities_)if(doomed.count(e.parent)&&doomed.insert(e.id).second)changed=true;}}else{for(auto&e:entities_)if(e.parent==id)e.parent=0;}for(auto it=entities_.begin();it!=entities_.end();){if(doomed.count(it->id)){index_.erase(it->id);it=entities_.erase(it);}else ++it;}return true;}
Entity* Scene::duplicate_entity(EntityId id,EntityId parent_override){const Entity*src=find(id);if(!src)return nullptr;const EntityId dst=parent_override?parent_override:src->parent;if(dst&&!find(dst))return nullptr;Entity&copy=create_entity(src->name+" Copy",dst);copy.transform=src->transform;copy.sprite=src->sprite;copy.layer=src->layer;copy.active=src->active;copy.visible=src->visible;copy.locked=src->locked;const auto children=children_of(id);for(EntityId child:children){if(full())break;duplicate_entity(child,copy.id);}return &copy;}
void Scene::clear(){entities_.clear();index_.clear();next_id_=1;} void Scene::reset_id_counter(EntityId next_id){EntityId candidate=std::max<EntityId>(1,next_id);if(!entities_.empty()){auto it=std::max_element(entities_.begin(),entities_.end(),[](const Entity&a,const Entity&b){return a.id<b.id;});candidate=std::max(candidate,it->id+1);}next_id_=candidate?candidate:1;}
}
