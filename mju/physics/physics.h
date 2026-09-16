#pragma once
#include "collision.h"
#include "../core/scene.h"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace mju::physics {
enum class BodyType { Static, Dynamic, Kinematic };
struct Body {
    EntityId entity=0;
    BodyType type=BodyType::Static;
    Vec2 velocity{};
    Vec2 acceleration{};
    float gravityScale=1.0f;
    float mass=1.0f;
    float restitution=0.0f;
    float friction=0.5f;
    bool useGravity=true;
    bool fixedRotation=false;
};

class World {
public:
    Body& add_body(EntityId id, BodyType type=BodyType::Dynamic);
    Body* get_body(EntityId id); const Body* get_body(EntityId id) const;
    void remove_body(EntityId id); void clear();
    void step(Scene& scene,float dt,Vec2 gravity={0,980});
    bool overlaps(const Scene& scene,EntityId a,EntityId b) const;
    std::size_t body_count() const { return bodies_.size(); }
private:
    std::unordered_map<EntityId,Body> bodies_;
};
}
