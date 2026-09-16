#include "sprite_batch.h"
#include <algorithm>
#include <cmath>

namespace mju::render {

SpriteBatch::SpriteBatch(std::size_t capacity) : capacity_(std::max<std::size_t>(1, capacity)) {
    reserve(capacity_);
}

void SpriteBatch::reserve(std::size_t capacity) {
    capacity_ = std::max<std::size_t>(1, capacity);
    queued_.reserve(capacity_);
    vertices_.reserve(capacity_ * 4);
    indices_.reserve(capacity_ * 6);
}

void SpriteBatch::begin() {
    queued_.clear();
    vertices_.clear();
    indices_.clear();
    stats_ = {};
    active_ = true;
}

void SpriteBatch::submit(const SpriteQuad& quad) {
    if (!active_) return;
    if (queued_.size() < capacity_) {
        queued_.push_back(quad);
    }
}

void SpriteBatch::end() {
    if (!active_) return;
    rebuild_geometry();
    active_ = false;
}

void SpriteBatch::rebuild_geometry() {
    std::stable_sort(queued_.begin(), queued_.end(), [](const SpriteQuad& a, const SpriteQuad& b) {
        if (a.layer != b.layer) return a.layer < b.layer;
        return a.texture_id < b.texture_id;
    });

    vertices_.clear();
    indices_.clear();
    vertices_.reserve(queued_.size() * 4);
    indices_.reserve(queued_.size() * 6);

    std::uint32_t base = 0;
    std::uint32_t last_texture = 0xffffffffu;
    stats_.sprites = queued_.size();
    stats_.batches = 0;

    for (const auto& q : queued_) {
        if (q.texture_id != last_texture) {
            ++stats_.batches;
            last_texture = q.texture_id;
        }

        const float hx = q.size.x * 0.5f;
        const float hy = q.size.y * 0.5f;
        const float r = q.rotation * 3.14159265358979323846f / 180.0f;
        const float c = std::cos(r);
        const float s = std::sin(r);

        const Vec2 local[4] = {{-hx,-hy},{hx,-hy},{hx,hy},{-hx,hy}};
        const Vec2 uv[4] = {
            {q.uv_min.x,q.uv_min.y}, {q.uv_max.x,q.uv_min.y},
            {q.uv_max.x,q.uv_max.y}, {q.uv_min.x,q.uv_max.y}
        };

        for (int i=0;i<4;++i) {
            const float x = local[i].x;
            const float y = local[i].y;
            vertices_.push_back({
                {q.position.x + x*c - y*s, q.position.y + x*s + y*c},
                uv[i], q.color
            });
        }

        indices_.insert(indices_.end(), {base, base+1, base+2, base, base+2, base+3});
        base += 4;
    }

    stats_.vertices = vertices_.size();
    stats_.indices = indices_.size();
}

}
