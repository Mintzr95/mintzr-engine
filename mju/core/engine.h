#pragma once
#include "scene.h"
#include "camera.h"
#include "input.h"
#include "animation.h"
#include "../audio/audio.h"
#include "../physics/physics.h"
#include "../particles/particles.h"
#include "../resources/resources.h"
#include "../ui/ui.h"
#include "../scripting/script.h"
#include "../diagnostics/debug.h"
#include "../project/project.h"
namespace mju {
class Engine {
public:
 bool initialize(int w,int h); void resize(int w,int h); void update(float dt); void shutdown();
 Scene& scene(){return scene_;} const Scene& scene()const{return scene_;}
 Camera2D& camera(){return camera_;} const Camera2D& camera()const{return camera_;}
 InputState& input(){return input_;} const InputState& input()const{return input_;}
 AudioSystem& audio(){return audio_;}
 physics::World& physics(){return physics_;}
 particles::Emitter& particles(){return particles_;}
 resources::ResourceCatalog& resources(){return resources_;}
 ui::Canvas& ui(){return ui_;} const ui::Canvas& ui()const{return ui_;}
 scripting::ScriptVM& scripts(){return scripts_;}
 debug::Console& console(){return console_;}
 project::Settings& settings(){return settings_;} const project::Settings& settings()const{return settings_;}
 int width()const{return width_;} int height()const{return height_;} bool initialized()const{return initialized_;} unsigned long long frame_count()const{return frame_count_;}
private:
 int width_=0,height_=0; bool initialized_=false; unsigned long long frame_count_=0; Scene scene_; Camera2D camera_; InputState input_; AudioSystem audio_; physics::World physics_; particles::Emitter particles_; resources::ResourceCatalog resources_; ui::Canvas ui_; scripting::ScriptVM scripts_; debug::Console console_; project::Settings settings_;
};
}
