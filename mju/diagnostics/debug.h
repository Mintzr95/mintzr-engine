#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace mju::debug {
class Console {
public:
 void info(std::string s); void warn(std::string s); void error(std::string s);
 const std::vector<std::string>& lines() const { return lines_; }
 void clear(){ lines_.clear(); }
 std::size_t frame() const { return frame_; } void next_frame(){ ++frame_; }
private: std::vector<std::string> lines_; std::size_t frame_=0;
};
}
