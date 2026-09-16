#pragma once
#include <cstdint>
#include <string>

namespace mju::project {
struct Settings {
    std::string name="MJU Game";
    std::string start_scene="main.mjuscene";
    int width=1280;
    int height=720;
    int target_fps=60;
    bool portrait=true;
    bool vsync=true;
    int max_entities=65536;
    std::string renderer="gles2";
};

bool save_settings(const Settings&, const std::string& path);
bool load_settings(Settings&, const std::string& path);
}
