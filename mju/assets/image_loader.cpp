#include "image_loader.h"

#if defined(MJU_HAS_STB_IMAGE)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

namespace mju::assets {

bool load_image_rgba(const std::string& path, Image& out) {
#if defined(MJU_HAS_STB_IMAGE)
    int w = 0, h = 0, channels = 0;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    if (!data || w <= 0 || h <= 0) {
        if (data) stbi_image_free(data);
        return false;
    }
    out.width = w;
    out.height = h;
    out.channels = 4;
    out.pixels.assign(data, data + static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 4u);
    stbi_image_free(data);
    return true;
#else
    (void)path;
    free_image(out);
    return false;
#endif
}

void free_image(Image& image) {
    image.width = 0;
    image.height = 0;
    image.channels = 4;
    image.pixels.clear();
    image.pixels.shrink_to_fit();
}

}
