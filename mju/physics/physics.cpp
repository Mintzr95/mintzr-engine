#include "physics.h"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace mju::physics {
namespace {
struct Aabb {
    Vec2 min{};
    Vec2 max{};
};

Aabb bounds(const Entity& e) {
    const Vec2 h{
        std::fabs(e.sprite.size.x * e.transform.scale.x) * 0.5f,
        std::fabs(e.sprite.size.y * e.transform.scale.y) * 0.5f
    };
    return {e.transform.position - h, e.transform.position + h};
}

bool overlap(const Aabb& a, const Aabb& b, float& px, float& py) {
    const float ox = std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x);
    const float oy = std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y);
    if (ox <= 0.0f || oy <= 0.0f) return false;
    px = ox;
    py = oy;
    return true;
}

std::int64_t cell_key(int x, int y) {
    return (static_cast<std::int64_t>(x) << 32) ^ static_cast<std::uint32_t>(y);
}

std::uint64_t pair_key(EntityId a, EntityId b) {
    if (a > b) std::swap(a, b);
    return (static_cast<std::uint64_t>(a) << 32) | b;
}
}

Body& World::add_body(EntityId id, BodyType type) {
    auto& body = bodies_[id];
    body.entity = id;
    body.type = type;
    return body;
}

Body* World::get_body(EntityId id) {
    auto it = bodies_.find(id);
    return it == bodies_.end() ? nullptr : &it->second;
}

const Body* World::get_body(EntityId id) const {
    auto it = bodies_.find(id);
    return it == bodies_.end() ? nullptr : &it->second;
}

void World::remove_body(EntityId id) { bodies_.erase(id); }
void World::clear() { bodies_.clear(); }

void World::notify_contact(const Contact& contact) const {
    if (contact_callback_) contact_callback_(contact);
}

void World::step(Scene& scene, float dt, Vec2 gravity) {
    step(scene, dt, gravity, nullptr);
}

void World::step(Scene& scene, float dt, Vec2 gravity, const tilemap::TileMap* map) {
    if (dt <= 0.0f) return;

    constexpr float cell = 128.0f;
    std::unordered_map<std::int64_t, std::vector<EntityId>> grid;
    grid.reserve(bodies_.size() * 2 + 1);

    for (auto& [id, body] : bodies_) {
        auto* entity = scene.find(id);
        if (!entity || !entity->active) continue;

        if (body.type == BodyType::Dynamic) {
            if (body.useGravity) body.acceleration = gravity * body.gravityScale;
            body.velocity += body.acceleration * dt;
            entity->transform.position += body.velocity * dt;
        } else if (body.type == BodyType::Kinematic) {
            entity->transform.position += body.velocity * dt;
        }
        body.acceleration = {0, 0};

        const Aabb a = bounds(*entity);
        const int x0 = static_cast<int>(std::floor(a.min.x / cell));
        const int y0 = static_cast<int>(std::floor(a.min.y / cell));
        const int x1 = static_cast<int>(std::floor(a.max.x / cell));
        const int y1 = static_cast<int>(std::floor(a.max.y / cell));
        for (int y = y0; y <= y1; ++y) {
            for (int x = x0; x <= x1; ++x) {
                grid[cell_key(x, y)].push_back(id);
            }
        }
    }

    std::unordered_set<std::uint64_t> tested;
    tested.reserve(bodies_.size() * 2 + 1);

    for (const auto& [unused, ids] : grid) {
        (void)unused;
        for (std::size_t i = 0; i < ids.size(); ++i) {
            for (std::size_t j = i + 1; j < ids.size(); ++j) {
                const EntityId a_id = ids[i];
                const EntityId b_id = ids[j];
                if (!tested.insert(pair_key(a_id, b_id)).second) continue;

                auto* a = scene.find(a_id);
                auto* b = scene.find(b_id);
                if (!a || !b || !a->active || !b->active) continue;

                Body* body_a = get_body(a_id);
                Body* body_b = get_body(b_id);
                if (!body_a || !body_b) continue;
                if (body_a->type == BodyType::Static && body_b->type == BodyType::Static) continue;

                float px = 0.0f;
                float py = 0.0f;
                if (!overlap(bounds(*a), bounds(*b), px, py)) continue;

                const Vec2 delta = b->transform.position - a->transform.position;
                const bool x_axis = px < py;
                const float penetration = x_axis ? px : py;
                const Vec2 normal = x_axis
                    ? Vec2{delta.x >= 0.0f ? 1.0f : -1.0f, 0.0f}
                    : Vec2{0.0f, delta.y >= 0.0f ? 1.0f : -1.0f};

                const bool dynamic_a = body_a->type == BodyType::Dynamic;
                const bool dynamic_b = body_b->type == BodyType::Dynamic;
                if (dynamic_a && dynamic_b) {
                    a->transform.position -= normal * (penetration * 0.5f);
                    b->transform.position += normal * (penetration * 0.5f);
                } else if (dynamic_a) {
                    a->transform.position -= normal * penetration;
                } else if (dynamic_b) {
                    b->transform.position += normal * penetration;
                }

                const float relative_normal =
                    (body_b->velocity - body_a->velocity).x * normal.x +
                    (body_b->velocity - body_a->velocity).y * normal.y;
                const float inv_a = dynamic_a ? 1.0f / std::max(0.001f, body_a->mass) : 0.0f;
                const float inv_b = dynamic_b ? 1.0f / std::max(0.001f, body_b->mass) : 0.0f;
                const float inv_sum = inv_a + inv_b;

                if (relative_normal < 0.0f && inv_sum > 0.0f) {
                    const float restitution = std::clamp(
                        std::max(body_a->restitution, body_b->restitution), 0.0f, 1.0f);
                    const float impulse_magnitude =
                        -(1.0f + restitution) * relative_normal / inv_sum;
                    const Vec2 impulse = normal * impulse_magnitude;
                    if (dynamic_a) body_a->velocity -= impulse * inv_a;
                    if (dynamic_b) body_b->velocity += impulse * inv_b;

                    const Vec2 tangent{-normal.y, normal.x};
                    const Vec2 rv = body_b->velocity - body_a->velocity;
                    const float relative_tangent = rv.x * tangent.x + rv.y * tangent.y;
                    const float friction = std::clamp(
                        std::sqrt(std::max(0.0f, body_a->friction * body_b->friction)),
                        0.0f,
                        1.0f);
                    const float friction_impulse =
                        std::clamp(-relative_tangent / inv_sum,
                                   -impulse_magnitude * friction,
                                   impulse_magnitude * friction);
                    const Vec2 friction_vector = tangent * friction_impulse;
                    if (dynamic_a) body_a->velocity -= friction_vector * inv_a;
                    if (dynamic_b) body_b->velocity += friction_vector * inv_b;
                }

                notify_contact({a_id, b_id, normal, penetration, 0, 0, false});
            }
        }
    }

    if (!map || map->width() <= 0 || map->height() <= 0) return;
    const int tile_width = std::max(1, map->tile_width());
    const int tile_height = std::max(1, map->tile_height());

    for (auto& [id, body] : bodies_) {
        if (body.type != BodyType::Dynamic) continue;
        auto* entity = scene.find(id);
        if (!entity || !entity->active) continue;

        for (int pass = 0; pass < 4; ++pass) {
            const Aabb entity_box = bounds(*entity);
            const int x0 = std::max(0, static_cast<int>(std::floor(entity_box.min.x / tile_width)) - 1);
            const int y0 = std::max(0, static_cast<int>(std::floor(entity_box.min.y / tile_height)) - 1);
            const int x1 = std::min(map->width() - 1,
                                    static_cast<int>(std::floor(entity_box.max.x / tile_width)) + 1);
            const int y1 = std::min(map->height() - 1,
                                    static_cast<int>(std::floor(entity_box.max.y / tile_height)) + 1);
            bool resolved = false;

            for (int y = y0; y <= y1 && !resolved; ++y) {
                for (int x = x0; x <= x1 && !resolved; ++x) {
                    if (map->get(x, y).id < 0) continue;

                    const Aabb tile_box{
                        {static_cast<float>(x * tile_width), static_cast<float>(y * tile_height)},
                        {static_cast<float>((x + 1) * tile_width), static_cast<float>((y + 1) * tile_height)}};
                    float px = 0.0f;
                    float py = 0.0f;
                    if (!overlap(entity_box, tile_box, px, py)) continue;

                    const Vec2 tile_center{
                        (x + 0.5f) * tile_width,
                        (y + 0.5f) * tile_height
                    };
                    const Vec2 delta = tile_center - entity->transform.position;
                    const bool x_axis = px < py;
                    const float penetration = x_axis ? px : py;
                    const Vec2 normal = x_axis
                        ? Vec2{delta.x >= 0.0f ? 1.0f : -1.0f, 0.0f}
                        : Vec2{0.0f, delta.y >= 0.0f ? 1.0f : -1.0f};

                    entity->transform.position -= normal * penetration;
                    const float normal_velocity =
                        body.velocity.x * normal.x + body.velocity.y * normal.y;
                    if (normal_velocity > 0.0f) {
                        body.velocity -= normal * normal_velocity;
                    }
                    if (body.restitution > 0.0f && normal_velocity < 0.0f) {
                        body.velocity -= normal * (normal_velocity * (1.0f + body.restitution));
                    }

                    notify_contact({id, 0, normal, penetration, x, y, true});
                    resolved = true;
                }
            }
            if (!resolved) break;
        }
    }
}

bool World::overlaps(const Scene& scene, EntityId a, EntityId b) const {
    auto* entity_a = scene.find(a);
    auto* entity_b = scene.find(b);
    if (!entity_a || !entity_b) return false;
    float px = 0.0f;
    float py = 0.0f;
    return overlap(bounds(*entity_a), bounds(*entity_b), px, py);
}
}
