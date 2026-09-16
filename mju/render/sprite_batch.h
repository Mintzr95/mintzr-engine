#pragma once
#include "../core/math.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mju::render {
struct SpriteVertex { Vec2 position{}; Vec2 uv{}; Color color{}; };
struct SpriteQuad {
    Vec2 position{}; Vec2 size{1,1}; Vec2 uv_min{0,0}; Vec2 uv_max{1,1};
    float rotation=0.0f; Color color{1,1,1,1}; std::uint32_t texture_id=0; int layer=0;
};
struct BatchRange { std::uint32_t texture_id=0; std::size_t first_index=0; std::size_t index_count=0; };
struct BatchStats { std::size_t sprites=0; std::size_t batches=0; std::size_t vertices=0; std::size_t indices=0; };
class SpriteBatch {
public:
    explicit SpriteBatch(std::size_t capacity=2048);
    void begin(); void submit(const SpriteQuad& quad); void end(); void reserve(std::size_t capacity);
    const std::vector<SpriteVertex>& vertices() const{return vertices_;}
    const std::vector<std::uint32_t>& indices() const{return indices_;}
    const std::vector<BatchRange>& ranges() const{return ranges_;}
    const BatchStats& stats() const{return stats_;}
private:
    std::vector<SpriteVertex> vertices_; std::vector<std::uint32_t> indices_; std::vector<SpriteQuad> queued_; std::vector<BatchRange> ranges_;
    BatchStats stats_{}; std::size_t capacity_; bool active_=false; void rebuild_geometry();
};
}
