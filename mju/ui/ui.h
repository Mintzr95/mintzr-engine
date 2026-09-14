#pragma once
#include "../core/math.h"
#include <string>
#include <vector>
#include <functional>

namespace mju::ui {

enum class Anchor { TopLeft, Top, TopRight, Left, Center, Right, BottomLeft, Bottom, BottomRight, Stretch };
struct Rect { Vec2 position{}; Vec2 size{100,40}; };
struct Widget {
    std::string id;
    Rect rect{};
    Anchor anchor=Anchor::TopLeft;
    bool visible=true;
    bool enabled=true;
    bool interactable=true;
    std::string text;
    std::function<void()> on_click{};
};

class Canvas {
public:
    Widget& add(std::string id,std::string text="");
    Widget* find(const std::string& id);
    const std::vector<Widget>& widgets() const { return widgets_; }
    void clear(){widgets_.clear();}
    void layout(float width,float height);
    Widget* hit_test(Vec2 point);
    bool pointer_down(Vec2 point);
private:
    std::vector<Widget> widgets_;
};

} // namespace mju::ui
