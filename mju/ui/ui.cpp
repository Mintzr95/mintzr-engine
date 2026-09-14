#include "ui.h"
namespace mju::ui {
Widget& Canvas::add(std::string id,std::string text){ widgets_.push_back(Widget{std::move(id),{},Anchor::TopLeft,true,true,true,std::move(text),{}}); return widgets_.back(); }
Widget* Canvas::find(const std::string& id){ for(auto&w:widgets_) if(w.id==id) return &w; return nullptr; }
void Canvas::layout(float width,float height){
    for(auto&w:widgets_){
        const float half_w=w.rect.size.x*0.5f, half_h=w.rect.size.y*0.5f;
        switch(w.anchor){
            case Anchor::TopLeft: break;
            case Anchor::Top: w.rect.position={width*0.5f,half_h}; break;
            case Anchor::TopRight: w.rect.position={width-half_w,half_h}; break;
            case Anchor::Left: w.rect.position={half_w,height*0.5f}; break;
            case Anchor::Center: w.rect.position={width*0.5f,height*0.5f}; break;
            case Anchor::Right: w.rect.position={width-half_w,height*0.5f}; break;
            case Anchor::BottomLeft: w.rect.position={half_w,height-half_h}; break;
            case Anchor::Bottom: w.rect.position={width*0.5f,height-half_h}; break;
            case Anchor::BottomRight: w.rect.position={width-half_w,height-half_h}; break;
            case Anchor::Stretch: w.rect.position={width*0.5f,height*0.5f}; w.rect.size={width,height}; break;
        }
    }
}
Widget* Canvas::hit_test(Vec2 p){
    for(auto it=widgets_.rbegin();it!=widgets_.rend();++it){auto&w=*it; if(!w.visible||!w.enabled||!w.interactable) continue; float hx=w.rect.size.x*0.5f,hy=w.rect.size.y*0.5f; if(p.x>=w.rect.position.x-hx&&p.x<=w.rect.position.x+hx&&p.y>=w.rect.position.y-hy&&p.y<=w.rect.position.y+hy)return &w;} return nullptr;
}
bool Canvas::pointer_down(Vec2 p){auto*w=hit_test(p);if(!w)return false;if(w->on_click)w->on_click();return true;}
}
