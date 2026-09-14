#include "scene_io.h"
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>

namespace mju {

bool save_scene(const Scene& s, const std::string& p) {
    std::ofstream f(p);
    if(!f) return false;
    f << "MJU2\n";
    f << "entities " << s.entities().size() << "\n";
    for(const auto& e: s.entities()) {
        f << e.id << ' ' << e.parent << ' ' << e.layer << ' '
          << e.active << ' ' << e.visible << ' ' << e.locked << ' '
          << e.transform.position.x << ' ' << e.transform.position.y << ' '
          << e.transform.scale.x << ' ' << e.transform.scale.y << ' '
          << e.transform.rotation << ' '
          << std::quoted(e.name) << ' ' << std::quoted(e.sprite.texture) << ' '
          << e.sprite.size.x << ' ' << e.sprite.size.y << ' '
          << e.sprite.color.r << ' ' << e.sprite.color.g << ' ' << e.sprite.color.b << ' ' << e.sprite.color.a << ' '
          << e.sprite.visible << '\n';
    }
    return static_cast<bool>(f);
}

bool load_scene(Scene& s, const std::string& p) {
    std::ifstream f(p);
    if(!f) return false;
    std::string header;
    if(!(f>>header) || header!="MJU2") return false;
    std::string token; std::size_t count=0;
    if(!(f>>token) || token!="entities" || !(f>>count)) return false;

    Scene loaded;
    EntityId max_id=0;
    for(std::size_t i=0;i<count;++i) {
        Entity e;
        if(!(f >> e.id >> e.parent >> e.layer >> e.active >> e.visible >> e.locked
             >> e.transform.position.x >> e.transform.position.y
             >> e.transform.scale.x >> e.transform.scale.y >> e.transform.rotation
             >> std::quoted(e.name) >> std::quoted(e.sprite.texture)
             >> e.sprite.size.x >> e.sprite.size.y
             >> e.sprite.color.r >> e.sprite.color.g >> e.sprite.color.b >> e.sprite.color.a
             >> e.sprite.visible)) return false;
        max_id=std::max(max_id,e.id);
        loaded.entities().push_back(std::move(e));
    }
    loaded.reset_id_counter(max_id+1);
    s=std::move(loaded);
    return true;
}
}
