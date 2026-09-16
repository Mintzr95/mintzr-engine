#pragma once
#include <cstddef>
#include <cstdint>
#include <string>

namespace mju::performance {

struct MobileProfile {
    std::uint32_t logical_width=1280;
    std::uint32_t logical_height=720;
    std::uint32_t target_fps=60;
    std::uint32_t max_sprites_per_batch=2048;
    std::uint32_t max_texture_size=2048;
    std::uint32_t max_particles=4096;
    std::uint32_t resource_cache_entries=512;
    std::uint32_t worker_threads=2;
    std::size_t transient_vertex_bytes=2u * 1024u * 1024u;
    bool prefer_gles2=true;
    bool allow_gles3=true;
    bool allow_vulkan=false;
    bool texture_mipmaps=false;
    bool vsync=true;
};

MobileProfile low_end();
MobileProfile balanced();
MobileProfile high_end();
std::string to_string(const MobileProfile& profile);

}
