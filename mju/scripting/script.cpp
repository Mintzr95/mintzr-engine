#include "script.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <sstream>

namespace mju::scripting {

namespace {
constexpr std::size_t kMaxOperations = 4096;
constexpr std::size_t kMaxLoopIterations = 256;
constexpr int kMaxCallDepth = 16;

bool compare(const std::string& op, float a, float b) {
    if (op == "==") return a == b;
    if (op == "!=") return a != b;
    if (op == "<") return a < b;
    if (op == "<=") return a <= b;
    if (op == ">") return a > b;
    if (op == ">=") return a >= b;
    return false;
}
}

std::vector<std::string> ScriptVM::split(const std::string& line) {
    std::istringstream stream(line);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) tokens.push_back(token);
    return tokens;
}

std::string ScriptVM::trim(const std::string& line) {
    const auto first = line.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = line.find_last_not_of(" \t\r\n");
    return line.substr(first, last - first + 1);
}

bool ScriptVM::is_comment_or_empty(const std::string& line) {
    const std::string value = trim(line);
    return value.empty() || value[0] == '#';
}

ScriptVM::Lines ScriptVM::collect_block(const std::vector<std::string>& lines,
                                        std::size_t& index,
                                        const std::string& end_token) {
    Lines block;
    while (index < lines.size()) {
        const std::string line = trim(lines[index++]);
        if (line == end_token) return block;
        block.push_back(line);
    }
    return {};
}

bool ScriptVM::parse_blocks(const std::string& source,
                            Lines& main_lines,
                            std::unordered_map<std::string, Lines>& functions,
                            std::unordered_map<std::string, Lines>& events,
                            std::string& error) {
    std::vector<std::string> lines;
    std::istringstream stream(source);
    std::string line;
    while (std::getline(stream, line)) lines.push_back(trim(line));

    for (std::size_t index = 0; index < lines.size();) {
        const std::string current = lines[index++];
        if (is_comment_or_empty(current)) continue;
        const auto tokens = split(current);
        if (tokens.empty()) continue;

        if ((tokens[0] == "fn" || tokens[0] == "event") && tokens.size() == 2) {
            const std::string name = tokens[1];
            const std::string end_token = tokens[0] == "fn" ? "endfn" : "endevent";
            Lines block;
            bool found_end = false;
            while (index < lines.size()) {
                const std::string body_line = trim(lines[index++]);
                if (body_line == end_token) {
                    found_end = true;
                    break;
                }
                block.push_back(body_line);
            }
            if (!found_end) {
                error = "missing_" + end_token;
                return false;
            }

            auto& table = tokens[0] == "fn" ? functions : events;
            if (!table.emplace(name, std::move(block)).second) {
                error = "duplicate_" + tokens[0] + "_" + name;
                return false;
            }
            continue;
        }

        if (tokens[0] == "endfn" || tokens[0] == "endevent") {
            error = "unexpected_" + tokens[0];
            return false;
        }
        main_lines.push_back(current);
    }
    return true;
}

void ScriptVM::clear_state(EntityId entity) {
    if (entity == 0) states_.clear();
    else states_.erase(entity);
}

ScriptResult ScriptVM::execute(Scene& scene,
                               EntityId self,
                               const std::string& source,
                               ScriptContext context) {
    Entity* entity = scene.find(self);
    if (!entity) return {false, "entity_not_found", 0};

    Lines main_lines;
    std::unordered_map<std::string, Lines> functions;
    std::unordered_map<std::string, Lines> events;
    std::string parse_error;
    if (!parse_blocks(source, main_lines, functions, events, parse_error)) {
        return {false, parse_error, 0};
    }

    auto& state = states_[self];
    std::size_t operations = 0;
    int call_depth = 0;
    std::string failure_reason = "invalid_or_unsafe_command";

    auto value = [&](const std::string& token, float fallback = 0.0f) {
        const auto it = state.variables.find(token);
        if (it != state.variables.end()) return it->second;
        char* end = nullptr;
        const float parsed = std::strtof(token.c_str(), &end);
        return end && *end == '\0' ? parsed : fallback;
    };

    std::function<bool(const Lines&)> run_lines;
    std::function<bool(const std::vector<std::string>&)> run_command;

    run_command = [&](const std::vector<std::string>& tokens) -> bool {
        if (tokens.empty()) return true;
        if (++operations > kMaxOperations) {
            failure_reason = "execution_limit";
            return false;
        }

        const std::string& command = tokens[0];
        auto number = [&](std::size_t index, float fallback = 0.0f) {
            return index < tokens.size() ? value(tokens[index], fallback) : fallback;
        };

        if (command == "set" && tokens.size() >= 3) {
            state.variables[tokens[1]] = number(2);
            return true;
        }
        if (command == "add" && tokens.size() >= 3) {
            state.variables[tokens[1]] += number(2);
            return true;
        }
        if (command == "sub" && tokens.size() >= 3) {
            state.variables[tokens[1]] -= number(2);
            return true;
        }
        if (command == "mul" && tokens.size() >= 3) {
            state.variables[tokens[1]] *= number(2);
            return true;
        }
        if (command == "div" && tokens.size() >= 3) {
            const float divisor = number(2);
            if (std::fabs(divisor) < 1e-7f) {
                failure_reason = "division_by_zero";
                return false;
            }
            state.variables[tokens[1]] /= divisor;
            return true;
        }

        if ((command == "if" || command == "while") && tokens.size() >= 5) {
            const bool condition = compare(tokens[2], number(1), number(3));
            if (command == "if") {
                if (!condition) return true;
                return run_command({tokens.begin() + 4, tokens.end()});
            }
            const std::vector<std::string> body(tokens.begin() + 4, tokens.end());
            std::size_t iterations = 0;
            while (compare(tokens[2], number(1), number(3))) {
                if (++iterations > kMaxLoopIterations) {
                    failure_reason = "loop_limit";
                    return false;
                }
                if (!run_command(body)) return false;
            }
            return true;
        }

        if (command == "repeat" && tokens.size() >= 3) {
            const auto count = static_cast<std::size_t>(std::clamp(number(1), 0.0f,
                                                                     static_cast<float>(kMaxLoopIterations)));
            const std::vector<std::string> body(tokens.begin() + 2, tokens.end());
            for (std::size_t i = 0; i < count; ++i) {
                if (!run_command(body)) return false;
            }
            return true;
        }

        if (command == "call" && tokens.size() == 2) {
            const auto it = functions.find(tokens[1]);
            if (it == functions.end()) {
                failure_reason = "function_not_found";
                return false;
            }
            if (++call_depth > kMaxCallDepth) {
                --call_depth;
                failure_reason = "call_depth_limit";
                return false;
            }
            const bool ok = run_lines(it->second);
            --call_depth;
            return ok;
        }

        if (command == "move" && tokens.size() >= 3) {
            entity->transform.position.x += number(1);
            entity->transform.position.y += number(2);
            return true;
        }
        if (command == "setpos" && tokens.size() >= 3) {
            entity->transform.position = {number(1), number(2)};
            return true;
        }
        if (command == "rotate" && tokens.size() >= 2) {
            if (entity->locked) {
                failure_reason = "entity_locked";
                return false;
            }
            entity->transform.rotation += number(1);
            return true;
        }
        if (command == "scale" && tokens.size() >= 3) {
            entity->transform.scale = {number(1, 1), number(2, 1)};
            return true;
        }
        if (command == "visible" && tokens.size() >= 2) {
            entity->visible = number(1) != 0.0f;
            entity->sprite.visible = entity->visible;
            return true;
        }
        if (command == "color" && tokens.size() >= 4) {
            entity->sprite.color.r = number(1);
            entity->sprite.color.g = number(2);
            entity->sprite.color.b = number(3);
            if (tokens.size() >= 5) entity->sprite.color.a = number(4, 1);
            return true;
        }
        if (command == "name" && tokens.size() >= 2) {
            std::string joined = tokens[1];
            for (std::size_t i = 2; i < tokens.size(); ++i) joined += ' ' + tokens[i];
            entity->name = joined;
            return true;
        }

        if (command == "velocity" && tokens.size() >= 3 && context.physics) {
            auto* body = context.physics->get_body(self);
            if (!body) {
                failure_reason = "physics_body_not_found";
                return false;
            }
            body->velocity = {number(1), number(2)};
            return true;
        }
        if (command == "gravity_scale" && tokens.size() >= 2 && context.physics) {
            auto* body = context.physics->get_body(self);
            if (!body) {
                failure_reason = "physics_body_not_found";
                return false;
            }
            body->gravityScale = number(1, 1);
            return true;
        }
        if (command == "play" && tokens.size() >= 2 && context.audio) {
            if (!context.audio->play(tokens[1])) failure_reason = "audio_play_failed";
            return context.audio->playing(tokens[1]);
        }
        if (command == "sfx" && tokens.size() >= 2 && context.audio) {
            context.audio->play_sfx(tokens[1].c_str());
            return true;
        }

        failure_reason = "invalid_or_unsafe_command";
        return false;
    };

    run_lines = [&](const Lines& lines) -> bool {
        for (const std::string& raw_line : lines) {
            if (is_comment_or_empty(raw_line)) continue;
            if (!run_command(split(raw_line))) return false;
        }
        return true;
    };

    if (!run_lines(main_lines)) return {false, failure_reason, operations};
    return {true, "ok", operations};
}

ScriptResult ScriptVM::execute_event(Scene& scene,
                                     EntityId self,
                                     const std::string& source,
                                     const std::string& event,
                                     ScriptContext context) {
    Lines main_lines;
    std::unordered_map<std::string, Lines> functions;
    std::unordered_map<std::string, Lines> events;
    std::string parse_error;
    if (!parse_blocks(source, main_lines, functions, events, parse_error)) {
        return {false, parse_error, 0};
    }
    (void)main_lines;

    const auto event_it = events.find(event);
    if (event_it == events.end()) return {false, "event_not_found", 0};

    std::ostringstream event_program;
    for (const auto& [name, body] : functions) {
        event_program << "fn " << name << '\n';
        for (const auto& line : body) event_program << line << '\n';
        event_program << "endfn\n";
    }
    event_program << "fn __event_runner\n";
    for (const auto& line : event_it->second) event_program << line << '\n';
    event_program << "endfn\ncall __event_runner\n";
    return execute(scene, self, event_program.str(), context);
}

}
