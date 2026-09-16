#include "scene_manager.h"
#include <algorithm>

namespace mju {

Scene* SceneManager::create(const std::string& name) {
    if (name.empty()) return nullptr;
    if (auto* existing = get(name)) return existing;
    auto scene = std::make_unique<Scene>(max_entities_per_scene_);
    Scene* result = scene.get();
    scenes_.emplace(name, std::move(scene));
    if (current_name_.empty()) current_name_ = name;
    return result;
}

Scene* SceneManager::get(const std::string& name) {
    auto it = scenes_.find(name);
    return it == scenes_.end() ? nullptr : it->second.get();
}

const Scene* SceneManager::get(const std::string& name) const {
    auto it = scenes_.find(name);
    return it == scenes_.end() ? nullptr : it->second.get();
}

bool SceneManager::unload(const std::string& name) {
    auto it = scenes_.find(name);
    if (it == scenes_.end()) return false;
    scenes_.erase(it);
    if (current_name_ == name) {
        current_name_.clear();
        if (!scenes_.empty()) current_name_ = scenes_.begin()->first;
    }
    return true;
}

bool SceneManager::has(const std::string& name) const {
    return scenes_.find(name) != scenes_.end();
}

bool SceneManager::set_current(const std::string& name) {
    if (!has(name)) return false;
    current_name_ = name;
    return true;
}

Scene* SceneManager::current() {
    return current_name_.empty() ? nullptr : get(current_name_);
}

const Scene* SceneManager::current() const {
    return current_name_.empty() ? nullptr : get(current_name_);
}

std::vector<std::string> SceneManager::names() const {
    std::vector<std::string> result;
    result.reserve(scenes_.size());
    for (const auto& [name, scene] : scenes_) {
        (void)scene;
        result.push_back(name);
    }
    std::sort(result.begin(), result.end());
    return result;
}

void SceneManager::clear() {
    scenes_.clear();
    current_name_.clear();
}

}
