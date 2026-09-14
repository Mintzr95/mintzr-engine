#pragma once
#include "math.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace mju {
using EntityId = std::uint32_t;

struct Transform2D { Vec2 position{}; Vec2 scale{1,1}; float rotation=0; };
struct Sprite { Color color{}; Vec2 size{96,96}; bool visible=true; std::string texture; };

struct Entity {
    EntityId id=0;
    EntityId parent=0;
    std::string name;
    Transform2D transform{};
    Sprite sprite{};
    int layer=0;
    bool active=true;
    bool visible=true;
    bool locked=false;
};

class Scene {
public:
    Scene(){ entities_.reserve(1024); }
    Entity& create_entity(std::string name, EntityId parent=0);
    bool destroy_entity(EntityId id, bool destroy_children=true);
    Entity* duplicate_entity(EntityId id, EntityId parent_override=0);
    Entity* find(EntityId id);
    const Entity* find(EntityId id) const;
    std::vector<EntityId> children_of(EntityId parent) const;
    void clear();
    void reset_id_counter(EntityId next_id);
    EntityId next_id() const { return next_id_; }
    const std::vector<Entity>& entities() const { return entities_; }
    std::vector<Entity>& entities() { return entities_; }
private:
    std::vector<Entity> entities_;
    EntityId next_id_=1;
};
}
