#include "scene_io.h"
#include "scene_json.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <unordered_set>

namespace mju {
namespace {

bool load_legacy_mju2(Scene& output, const std::string& text) {
    std::istringstream f(text);
    std::string header;
    if (!(f >> header) || header != "MJU2") return false;

    std::string token;
    std::size_t count = 0;
    if (!(f >> token >> count) || token != "entities" || count > output.max_entities()) return false;

    Scene loaded(output.max_entities());
    std::vector<EntityId> ids;
    std::vector<EntityId> parents;
    ids.reserve(count);
    parents.reserve(count);
    std::unordered_set<EntityId> seen;
    EntityId next_id = 1;

    for (std::size_t i = 0; i < count; ++i) {
        Entity value;
        if (!(f >> value.id >> value.parent >> value.layer >> value.active >> value.visible >> value.locked
              >> value.transform.position.x >> value.transform.position.y
              >> value.transform.scale.x >> value.transform.scale.y >> value.transform.rotation
              >> std::quoted(value.name) >> std::quoted(value.sprite.texture)
              >> value.sprite.size.x >> value.sprite.size.y
              >> value.sprite.color.r >> value.sprite.color.g
              >> value.sprite.color.b >> value.sprite.color.a
              >> value.sprite.visible)) return false;
        if (!value.id || !seen.insert(value.id).second) return false;
        if (!loaded.create_entity_with_id(value.name, value.id, 0)) return false;
        auto* entity = loaded.find(value.id);
        *entity = value;
        ids.push_back(value.id);
        parents.push_back(value.parent);
        if (value.id == std::numeric_limits<EntityId>::max()) next_id = value.id;
        else next_id = std::max(next_id, static_cast<EntityId>(value.id + 1));
    }

    for (std::size_t i = 0; i < ids.size(); ++i) {
        if (parents[i] == ids[i]) return false;
        if (parents[i] && !loaded.find(parents[i])) return false;
        if (!loaded.set_parent(ids[i], parents[i])) return false;
    }
    loaded.reset_id_counter(next_id);
    output = std::move(loaded);
    return true;
}

} // namespace

bool save_scene(const Scene& s, const std::string& path) {
    return io::save_scene_json(s, path);
}

bool load_scene(Scene& s, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    if (text.empty()) return false;

    const std::size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return false;
    if (text.compare(first, 4, "MJU2") == 0) return load_legacy_mju2(s, text);
    return io::scene_from_json(s, text);
}

} // namespace mju
