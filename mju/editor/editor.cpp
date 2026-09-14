#include "editor.h"
#include "../io/scene_io.h"
#include <fstream>
#include <iomanip>
#include <cmath>

namespace mju::editor {
Entity* EditorState::create(const std::string& name, EntityId parent){ checkpoint(); Entity& e=scene.create_entity(name,parent); history.commit(scene); selected=e.id; return &e; }
bool EditorState::select(EntityId id){ if(!scene.find(id)) return false; selected=id; return true; }
Entity* EditorState::selected_entity(){ return selected ? scene.find(selected) : nullptr; }
bool EditorState::delete_selected(){ if(!selected) return false; checkpoint(); bool ok=scene.destroy_entity(selected,true); if(ok) { history.commit(scene); selected=0; } return ok; }
Entity* EditorState::duplicate_selected(){ if(!selected) return nullptr; checkpoint(); Entity* e=scene.duplicate_entity(selected); if(e) { history.commit(scene); selected=e->id; } return e; }
void EditorState::clear_selection(){ selected=0; }
void EditorState::toggle_play(){ playing=!playing; }
bool EditorState::save(const std::string& path){ return save_scene(scene,path); }
bool EditorState::load(const std::string& path){ bool ok=load_scene(scene,path); if(ok) { selected=0; history.reset(scene); } return ok; }
bool EditorState::undo(){ bool ok=history.undo(scene); if(ok) selected=0; return ok; }
bool EditorState::redo(){ bool ok=history.redo(scene); if(ok) selected=0; return ok; }
void EditorState::checkpoint(){ history.checkpoint(scene); }
void EditorState::snap_position(Entity& e){ if(!snap_to_grid) return; e.transform.position.x = std::round(e.transform.position.x/grid_size)*grid_size; e.transform.position.y = std::round(e.transform.position.y/grid_size)*grid_size; }

bool save_project(const ProjectSettings& s,const std::string& path){
    std::ofstream f(path); if(!f) return false;
    f<<"MJU_PROJECT 1\n"<<"name "<<std::quoted(s.name)<<"\n"<<"main_scene "<<std::quoted(s.main_scene)<<"\n"<<"width "<<s.width<<"\n"<<"height "<<s.height<<"\n"<<"portrait "<<s.portrait<<"\n";
    return static_cast<bool>(f);
}
bool load_project(ProjectSettings& s,const std::string& path){
    std::ifstream f(path); if(!f) return false;
    std::string header; int version=0; if(!(f>>header>>version)||header!="MJU_PROJECT") return false;
    std::string key; while(f>>key){ if(key=="name") f>>std::quoted(s.name); else if(key=="main_scene") f>>std::quoted(s.main_scene); else if(key=="width") f>>s.width; else if(key=="height") f>>s.height; else if(key=="portrait") f>>s.portrait; else {std::string ignored; std::getline(f,ignored);} }
    return true;
}
}
