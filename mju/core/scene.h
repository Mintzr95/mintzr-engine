#pragma once
#include "math.h"
#include <cstdint>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

namespace mju {
using EntityId = std::uint32_t;

struct Transform2D { Vec2 position{}; Vec2 scale{1,1}; float rotation=0; };
struct Sprite {
    Color color{};
    Vec2 size{96,96};
    bool visible=true;
    std::string texture;
    Vec2 uv_min{0,0};
    Vec2 uv_max{1,1};
    int frame_width=0;
    int frame_height=0;
    int frame=0;
    int frame_count=1;
    float fps=0.0f;
    float animation_time=0.0f;
    bool animation_loop=true;
    bool animation_playing=false;
};

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
    explicit Scene(std::size_t max_entities=65536);
    Entity& create_entity(std::string name, EntityId parent=0);
    bool destroy_entity(EntityId id, bool destroy_children=true);
    Entity* duplicate_entity(EntityId id, EntityId parent_override=0);
    Entity* find(EntityId id);
    const Entity* find(EntityId id) const;
    std::vector<EntityId> children_of(EntityId parent) const;
    Transform2D world_transform(EntityId id) const;
    bool set_parent(EntityId child, EntityId parent);
    void clear();
    void reset_id_counter(EntityId next_id);
    EntityId next_id() const { return next_id_; }
    std::size_t size() const { return entities_.size(); }
    std::size_t max_entities() const { return max_entities_; }
    bool full() const { return entities_.size() >= max_entities_; }
    const std::deque<Entity>& entities() const { return entities_; }
    std::deque<Entity>& entities() { return entities_; }

private:
    static Transform2D combine(const Transform2D& parent, const Transform2D& local);
    bool would_create_cycle(EntityId child, EntityId parent) const;
    std::deque<Entity> entities_;
    std::unordered_map<EntityId, Entity*> index_;
    EntityId next_id_=1;
    std::size_t max_entities_=65536;
};
}
