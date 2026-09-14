#pragma once
#include <string>
namespace mju::project {
struct Settings { std::string name="MJU Game"; int width=1280; int height=720; int target_fps=60; bool portrait=true; };
bool save_settings(const Settings&, const std::string& path);
bool load_settings(Settings&, const std::string& path);
}
