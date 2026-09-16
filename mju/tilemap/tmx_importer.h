#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace mju::tilemap {

struct ImportedTileLayer {
    std::string name;
    std::uint32_t width=0;
    std::uint32_t height=0;
    std::vector<std::uint32_t> tiles;
};

struct ImportedMap {
    std::uint32_t width=0;
    std::uint32_t height=0;
    std::uint32_t tile_width=0;
    std::uint32_t tile_height=0;
    std::vector<ImportedTileLayer> layers;
};

class TmxImporter {
public:
    bool load(const std::string& path, ImportedMap& out) const;
};

}
