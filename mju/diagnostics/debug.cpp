#include "debug.h"
namespace mju::debug {
void Console::info(std::string s){lines_.push_back("[INFO] "+std::move(s));}
void Console::warn(std::string s){lines_.push_back("[WARN] "+std::move(s));}
void Console::error(std::string s){lines_.push_back("[ERROR] "+std::move(s));}
}
