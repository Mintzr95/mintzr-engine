#pragma once
#include "../core/scene.h"
#include <vector>
namespace mju::editor {
class History {
public:
    void reset(const Scene& s){ undo_.clear(); redo_.clear(); current_=s; }
    void checkpoint(const Scene& s){ undo_.push_back(current_); redo_.clear(); }
    void commit(const Scene& s){ current_=s; }
    bool undo(Scene& s){ if(undo_.empty()) return false; redo_.push_back(current_); current_=undo_.back(); undo_.pop_back(); s=current_; return true; }
    bool redo(Scene& s){ if(redo_.empty()) return false; undo_.push_back(current_); current_=redo_.back(); redo_.pop_back(); s=current_; return true; }
    size_t undo_count() const{return undo_.size();}
    size_t redo_count() const{return redo_.size();}
private: Scene current_; std::vector<Scene> undo_,redo_;
};
}
