#include "application.h"
namespace mju { bool Application::start(int w,int h){return engine_.initialize(w,h);} void Application::resize(int w,int h){engine_.resize(w,h);} void Application::tick(float dt){engine_.update(dt);} void Application::stop(){engine_.shutdown();} }
