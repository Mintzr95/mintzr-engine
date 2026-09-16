#include "mobile_profile.h"
#include <sstream>

namespace mju::performance {

MobileProfile low_end() {
    MobileProfile p;
    p.logical_width=960; p.logical_height=540; p.target_fps=45;
    p.max_sprites_per_batch=768; p.max_texture_size=1024; p.max_particles=1200;
    p.resource_cache_entries=192; p.worker_threads=1; p.transient_vertex_bytes=512u*1024u;
    p.prefer_gles2=true; p.allow_gles3=false; p.allow_vulkan=false; p.texture_mipmaps=false;
    return p;
}

MobileProfile balanced() {
    return MobileProfile{};
}

MobileProfile high_end() {
    MobileProfile p;
    p.logical_width=1920; p.logical_height=1080; p.target_fps=60;
    p.max_sprites_per_batch=4096; p.max_texture_size=4096; p.max_particles=12000;
    p.resource_cache_entries=1024; p.worker_threads=3; p.transient_vertex_bytes=4u*1024u*1024u;
    p.prefer_gles2=false; p.allow_gles3=true; p.allow_vulkan=true; p.texture_mipmaps=true;
    return p;
}

std::string to_string(const MobileProfile& p) {
    std::ostringstream out;
    out << "width=" << p.logical_width << '\n'
        << "height=" << p.logical_height << '\n'
        << "fps=" << p.target_fps << '\n'
        << "sprites_per_batch=" << p.max_sprites_per_batch << '\n'
        << "texture_size=" << p.max_texture_size << '\n'
        << "particles=" << p.max_particles << '\n'
        << "cache_entries=" << p.resource_cache_entries << '\n'
        << "workers=" << p.worker_threads << '\n'
        << "transient_vertex_bytes=" << p.transient_vertex_bytes << '\n'
        << "gles2=" << (p.prefer_gles2?1:0) << '\n'
        << "gles3=" << (p.allow_gles3?1:0) << '\n'
        << "vulkan=" << (p.allow_vulkan?1:0) << '\n'
        << "mipmaps=" << (p.texture_mipmaps?1:0) << '\n'
        << "vsync=" << (p.vsync?1:0) << '\n';
    return out.str();
}

}
