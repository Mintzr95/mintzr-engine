#include "mju/core/engine.h"
#include "mju/io/scene_io.h"
#include <cassert>
#include <iostream>
int main(){
 mju::Engine e; assert(e.initialize(800,480));
 auto& a=e.scene().create_entity("A"); auto& b=e.scene().create_entity("B");
 assert(a.id!=b.id); e.update(1.0f/60.0f);
 std::string src="move 10 20\nrotate 5\ncolor 1 0 0 1";
 auto r=e.scripts().execute(e.scene(),a.id,src); assert(r.ok); assert(a.transform.position.x==10 && a.transform.position.y==20);
 const std::string path="mju_v1_test_scene.mju"; assert(mju::save_scene(e.scene(),path)); mju::Scene loaded; assert(mju::load_scene(loaded,path)); assert(loaded.entities().size()==e.scene().entities().size());
 std::remove(path.c_str()); e.shutdown(); std::cout<<"MJU v1.0 core test: PASS\n"; return 0;
}
