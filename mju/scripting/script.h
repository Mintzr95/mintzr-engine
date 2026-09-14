#pragma once
#include "../core/scene.h"
#include <string>
#include <vector>

namespace mju::scripting {

struct ScriptResult { bool ok=true; std::string message; };

class ScriptVM {
public:
    ScriptResult execute(Scene& scene, EntityId self, const std::string& source);
private:
    static std::vector<std::string> split(const std::string& line);
};

}
