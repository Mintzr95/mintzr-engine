#pragma once
#include "../core/scene.h"
#include <string>
namespace mju { bool save_scene(const Scene&, const std::string& path); bool load_scene(Scene&, const std::string& path); }
