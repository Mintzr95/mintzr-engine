#include "project.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace mju::project {
using json = nlohmann::json;

namespace {
json to_json(const Settings& settings) {
    return {
        {"format", "mju-project"},
        {"version", 2},
        {"name", settings.name},
        {"start_scene", settings.start_scene},
        {"width", settings.width},
        {"height", settings.height},
        {"target_fps", settings.target_fps},
        {"portrait", settings.portrait},
        {"vsync", settings.vsync},
        {"max_entities", settings.max_entities},
        {"renderer", settings.renderer}
    };
}

void clamp_settings(Settings& settings) {
    settings.width = std::clamp(settings.width, 64, 16384);
    settings.height = std::clamp(settings.height, 64, 16384);
    settings.target_fps = std::clamp(settings.target_fps, 15, 240);
    settings.max_entities = std::clamp(settings.max_entities, 1, 1000000);
    if (settings.start_scene.empty()) settings.start_scene = "main.mjuscene";
    if (settings.renderer.empty()) settings.renderer = "gles2";
}

bool load_legacy(std::istream& input, Settings& settings) {
    std::string header;
    int version = 0;
    if (!(input >> header >> version) || header != "MJU_PROJECT" || version != 1) return false;

    std::string key;
    while (input >> key) {
        if (key == "name") {
            input >> std::quoted(settings.name);
        } else if (key == "main_scene") {
            input >> std::quoted(settings.start_scene);
        } else if (key == "width") {
            input >> settings.width;
        } else if (key == "height") {
            input >> settings.height;
        } else if (key == "target_fps") {
            input >> settings.target_fps;
        } else if (key == "portrait") {
            input >> settings.portrait;
        } else if (key == "vsync") {
            input >> settings.vsync;
        } else if (key == "max_entities") {
            input >> settings.max_entities;
        } else if (key == "renderer") {
            input >> settings.renderer;
        } else {
            std::string ignored;
            std::getline(input, ignored);
        }
    }
    clamp_settings(settings);
    return static_cast<bool>(input) || input.eof();
}
}

bool save_settings(const Settings& input, const std::string& path) {
    if (path.empty()) return false;
    Settings settings = input;
    clamp_settings(settings);

    const std::string temporary_path = path + ".tmp";
    std::ofstream out(temporary_path, std::ios::binary | std::ios::trunc);
    if (!out) return false;
    out << to_json(settings).dump(2) << '\n';
    out.flush();
    if (!out) return false;
    out.close();
    std::remove(path.c_str());
    return std::rename(temporary_path.c_str(), path.c_str()) == 0;
}

bool load_settings(Settings& settings, const std::string& path) {
    if (path.empty()) return false;
    std::ifstream input(path, std::ios::binary);
    if (!input) return false;

    const std::string contents((std::istreambuf_iterator<char>(input)),
                               std::istreambuf_iterator<char>());
    if (contents.empty()) return false;

    const std::string trimmed = contents.substr(contents.find_first_not_of(" \t\r\n"));
    if (trimmed.rfind("MJU_PROJECT 1", 0) == 0) {
        std::istringstream legacy(contents);
        return load_legacy(legacy, settings);
    }

    try {
        const json root = json::parse(contents);
        if (root.value("format", "") != "mju-project") return false;
        const int version = root.value("version", 0);
        if (version < 2) return false;

        settings.name = root.value("name", settings.name);
        settings.start_scene = root.value("start_scene", settings.start_scene);
        settings.width = root.value("width", settings.width);
        settings.height = root.value("height", settings.height);
        settings.target_fps = root.value("target_fps", settings.target_fps);
        settings.portrait = root.value("portrait", settings.portrait);
        settings.vsync = root.value("vsync", settings.vsync);
        settings.max_entities = root.value("max_entities", settings.max_entities);
        settings.renderer = root.value("renderer", settings.renderer);
        clamp_settings(settings);
        return true;
    } catch (...) {
        return false;
    }
}
}
