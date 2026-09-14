#include "scene.h"
#include <unordered_set>

namespace mju {
Entity& Scene::create_entity(std::string name, EntityId parent) {
    entities_.push_back(Entity{});
    auto& e = entities_.back();
    e.id = next_id_++;
    e.parent = parent;
    e.name = std::move(name);
    return e;
}

Entity* Scene::find(EntityId id) { for(auto& e: entities_) if(e.id==id) return &e; return nullptr; }
const Entity* Scene::find(EntityId id) const { for(const auto& e: entities_) if(e.id==id) return &e; return nullptr; }

std::vector<EntityId> Scene::children_of(EntityId parent) const {
    std::vector<EntityId> out;
    for(const auto& e: entities_) if(e.parent==parent) out.push_back(e.id);
    return out;
}

bool Scene::destroy_entity(EntityId id, bool destroy_children) {
    if(!find(id)) return false;
    std::unordered_set<EntityId> doomed{ id };
    if(destroy_children) {
        bool changed=true;
        while(changed) {
            changed=false;
            for(const auto& e: entities_) {
                if(doomed.count(e.parent) && !doomed.count(e.id)) { doomed.insert(e.id); changed=true; }
            }
        }
    } else {
        for(auto& e: entities_) if(e.parent==id) e.parent=0;
    }
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(), [&](const Entity& e){ return doomed.count(e.id)>0; }), entities_.end());
    return true;
}

Entity* Scene::duplicate_entity(EntityId id, EntityId parent_override) {
    const Entity* src=find(id);
    if(!src) return nullptr;
    Entity& copy=create_entity(src->name+" Copy", parent_override ? parent_override : src->parent);
    copy.transform=src->transform;
    copy.sprite=src->sprite;
    copy.layer=src->layer;
    copy.active=src->active;
    copy.visible=src->visible;
    copy.locked=src->locked;
    return &copy;
}

void Scene::clear(){entities_.clear(); next_id_=1;}
void Scene::reset_id_counter(EntityId next_id){ next_id_=std::max<EntityId>(1,next_id); }
}
