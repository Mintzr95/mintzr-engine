#include "scene_json.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace mju::io {
using json = nlohmann::json;

static json entity_to_json(const Entity& e) {
    return {
        {"id", e.id}, {"parent", e.parent}, {"name", e.name},
        {"position", {e.transform.position.x, e.transform.position.y}},
        {"scale", {e.transform.scale.x, e.transform.scale.y}},
        {"rotation", e.transform.rotation},
        {"color", {e.sprite.color.r, e.sprite.color.g, e.sprite.color.b, e.sprite.color.a}},
        {"size", {e.sprite.size.x, e.sprite.size.y}},
        {"visible", e.sprite.visible}, {"texture", e.sprite.texture},
        {"layer", e.layer}, {"active", e.active}, {"entity_visible", e.visible}, {"locked", e.locked}
    };
}

std::string scene_to_json(const Scene& scene, int indent) {
    json root;
    root["format"] = "mju-scene";
    root["version"] = 1;
    root["next_id"] = scene.next_id();
    root["entities"] = json::array();
    for (const auto& e : scene.entities()) root["entities"].push_back(entity_to_json(e));
    return root.dump(indent);
}

bool scene_from_json(Scene& scene, const std::string& text) {
    try {
        const json root = json::parse(text);
        if (root.value("format", "") != "mju-scene") return false;
        if (root.value("version", 0) != 1) return false;
        scene.clear();
        EntityId max_id = 1;
        for (const auto& v : root.at("entities")) {
            const EntityId id = v.value("id", 0u);
            if (id == 0) return false;
            const EntityId parent = v.value("parent", 0u);
            auto& e = scene.create_entity(v.value("name", "Entity"), parent);
            e.id = id;
            auto p = v.value("position", std::vector<float>{0, 0});
            auto s = v.value("scale", std::vector<float>{1, 1});
            auto c = v.value("color", std::vector<float>{1, 1, 1, 1});
            auto sz = v.value("size", std::vector<float>{96, 96});
            if (p.size() >= 2) e.transform.position = {p[0], p[1]};
            if (s.size() >= 2) e.transform.scale = {s[0], s[1]};
            e.transform.rotation = v.value("rotation", 0.0f);
            if (c.size() >= 4) e.sprite.color = {c[0], c[1], c[2], c[3]};
            if (sz.size() >= 2) e.sprite.size = {sz[0], sz[1]};
            e.sprite.visible = v.value("visible", true);
            e.sprite.texture = v.value("texture", std::string{});
            e.layer = v.value("layer", 0);
            e.active = v.value("active", true);
            e.visible = v.value("entity_visible", true);
            e.locked = v.value("locked", false);
            if (id >= max_id) max_id = id + 1;
        }
        scene.reset_id_counter(std::max(max_id, root.value("next_id", max_id)));
        return true;
    } catch (...) {
        return false;
    }
}

bool save_scene_json(const Scene& scene, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out << scene_to_json(scene, 2);
    return static_cast<bool>(out);
}

bool load_scene_json(Scene& scene, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    return scene_from_json(scene, std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>()));
}

}
