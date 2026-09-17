#pragma once
#include "../core/math.h"
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

namespace mju::ui {

enum class Anchor {
    TopLeft, Top, TopRight,
    Left, Center, Right,
    BottomLeft, Bottom, BottomRight,
    Stretch
};

enum class LayoutMode {
    Absolute,
    Row,
    Column,
    Grid
};

struct Rect {
    Vec2 position{};
    Vec2 size{100, 40};
};

struct Widget {
    std::string id;
    Rect rect{};
    Anchor anchor = Anchor::TopLeft;
    bool visible = true;
    bool enabled = true;
    bool interactable = true;
    std::string text;
    std::function<void()> on_click{};

    std::string parent;
    LayoutMode layout_mode = LayoutMode::Absolute;
    float spacing = 8.0f;
    int columns = 1;
    Vec2 padding{};
};

class Canvas {
public:
    Widget& add(std::string id, std::string text = "");
    Widget* find(const std::string& id);
    const std::vector<Widget>& widgets() const { return widgets_; }
    void clear() { widgets_.clear(); }
    void layout(float width, float height);
    Widget* hit_test(Vec2 point);
    bool pointer_down(Vec2 point);

private:
    void layout_children(Widget& parent, std::unordered_set<std::string>& visiting);
    std::vector<Widget*> children_of(const std::string& parent_id);
    std::vector<Widget> widgets_;
};

} // namespace mju::ui
