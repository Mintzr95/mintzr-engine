#pragma once
#include "../core/scene.h"
#include "../audio/audio.h"
#include "../physics/physics.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace mju::scripting {

struct ScriptResult {
    bool ok = true;
    std::string message;
    std::size_t operations = 0;
};

struct ScriptContext {
    physics::World* physics = nullptr;
    AudioSystem* audio = nullptr;
};

class ScriptVM {
public:
    ScriptResult execute(Scene& scene,
                         EntityId self,
                         const std::string& source,
                         ScriptContext context = {});
    ScriptResult execute_event(Scene& scene,
                               EntityId self,
                               const std::string& source,
                               const std::string& event,
                               ScriptContext context = {});
    void clear_state(EntityId entity = 0);

private:
    struct EntityState {
        std::unordered_map<std::string, float> variables;
    };
    using Lines = std::vector<std::string>;

    static std::vector<std::string> split(const std::string& line);
    static std::string trim(const std::string& line);
    static bool is_comment_or_empty(const std::string& line);
    static Lines collect_block(const std::vector<std::string>& lines,
                               std::size_t& index,
                               const std::string& end_token);
    static bool parse_blocks(const std::string& source,
                             Lines& main_lines,
                             std::unordered_map<std::string, Lines>& functions,
                             std::unordered_map<std::string, Lines>& events,
                             std::string& error);

    std::unordered_map<EntityId, EntityState> states_;
};

}
