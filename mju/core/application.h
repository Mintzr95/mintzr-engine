#pragma once
#include "engine.h"
namespace mju { class Application { public: bool start(int w,int h); void resize(int w,int h); void tick(float dt); void stop(); Engine& engine(){return engine_;} private: Engine engine_; }; }
