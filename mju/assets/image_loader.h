#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace mju::assets {

struct Image {
    int width = 0;
    int height = 0;
    int channels = 4;
    std::vector<std::uint8_t> pixels;
    bool valid() const { return width > 0 && height > 0 && pixels.size() == static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4u; }
};

bool load_image_rgba(const std::string& path, Image& out);
void free_image(Image& image);

}
