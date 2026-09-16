#include "tmx_importer.h"
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/Layer.hpp>

namespace mju::tilemap {

bool TmxImporter::load(const std::string& path, ImportedMap& out) const {
    out = {};
    tmx::Map map;
    if (!map.load(path)) return false;

    const auto count = map.getTileCount();
    const auto size = map.getTileSize();
    out.width = count.x;
    out.height = count.y;
    out.tile_width = size.x;
    out.tile_height = size.y;

    for (const auto& layer_ptr : map.getLayers()) {
        if (!layer_ptr || layer_ptr->getType() != tmx::Layer::Type::Tile) continue;
        const auto& layer = layer_ptr->getLayerAs<tmx::TileLayer>();
        ImportedTileLayer converted;
        converted.name = layer.getName();
        converted.width = layer.getSize().x;
        converted.height = layer.getSize().y;
        converted.tiles.reserve(layer.getTiles().size());
        for (const auto& tile : layer.getTiles()) {
            converted.tiles.push_back(tile.ID);
        }
        out.layers.push_back(std::move(converted));
    }
    return true;
}

}
