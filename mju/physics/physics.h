#pragma once
#include "collision.h"
#include "../core/scene.h"
#include "../tilemap/tilemap.h"
#include <cstdint>
#include <functional>
#include <unordered_map>

namespace mju::physics {

enum class BodyType { Static, Dynamic, Kinematic };

struct Body {
    EntityId entity = 0;
    BodyType type = BodyType::Static;
    Vec2 velocity{};
    Vec2 acceleration{};
    float gravityScale = 1.0f;
    float mass = 1.0f;
    float restitution = 0.0f;
    float friction = 0.5f;
    bool useGravity = true;
    bool fixedRotation = false;
};

struct Contact {
    EntityId entity_a = 0;
    EntityId entity_b = 0;
    Vec2 normal{};
    float penetration = 0.0f;
    int tile_x = 0;
    int tile_y = 0;
    bool against_tilemap = false;
};

class World {
public:
    using ContactCallback = std::function<void(const Contact&)>;

    Body& add_body(EntityId id, BodyType type = BodyType::Dynamic);
    Body* get_body(EntityId id);
    const Body* get_body(EntityId id) const;
    void remove_body(EntityId id);
    void clear();

    void step(Scene& scene, float dt, Vec2 gravity = {0, 980});
    void step(Scene& scene, float dt, Vec2 gravity, const tilemap::TileMap* tilemap);
    bool overlaps(const Scene& scene, EntityId a, EntityId b) const;
    std::size_t body_count() const { return bodies_.size(); }

    void set_contact_callback(ContactCallback callback) { contact_callback_ = std::move(callback); }

private:
    void notify_contact(const Contact& contact) const;

    std::unordered_map<EntityId, Body> bodies_;
    ContactCallback contact_callback_;
};
}
