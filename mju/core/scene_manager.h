#pragma once
#include "scene.h"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mju {

class SceneManager {
public:
    SceneManager() = default;
    explicit SceneManager(std::size_t max_entities_per_scene)
        : max_entities_per_scene_(max_entities_per_scene) {}

    Scene* create(const std::string& name);
    Scene* get(const std::string& name);
    const Scene* get(const std::string& name) const;
    bool unload(const std::string& name);
    bool has(const std::string& name) const;
    bool set_current(const std::string& name);
    Scene* current();
    const Scene* current() const;
    const std::string& current_name() const { return current_name_; }
    std::vector<std::string> names() const;
    void clear();
    std::size_t size() const { return scenes_.size(); }

private:
    std::unordered_map<std::string, std::unique_ptr<Scene>> scenes_;
    std::string current_name_;
    std::size_t max_entities_per_scene_ = 65536;
};

}
