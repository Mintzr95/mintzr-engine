#pragma once
#include <string>
#include "../core/scene.h"

namespace mju::io {

bool save_scene_json(const Scene& scene, const std::string& path);
bool load_scene_json(Scene& scene, const std::string& path);
std::string scene_to_json(const Scene& scene, int indent = 2);
bool scene_from_json(Scene& scene, const std::string& json_text);

}
