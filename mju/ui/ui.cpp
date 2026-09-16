#include "ui.h"
#include <algorithm>
#include <unordered_set>

namespace mju::ui {

Widget& Canvas::add(std::string id, std::string text) {
    widgets_.push_back(Widget{std::move(id), {}, Anchor::TopLeft, true, true, true,
                               std::move(text), {}, {}, LayoutMode::Absolute, 8.0f, 1, {}});
    return widgets_.back();
}

Widget* Canvas::find(const std::string& id) {
    for (auto& widget : widgets_) {
        if (widget.id == id) return &widget;
    }
    return nullptr;
}

std::vector<Widget*> Canvas::children_of(const std::string& parent_id) {
    std::vector<Widget*> result;
    for (auto& widget : widgets_) {
        if (widget.parent == parent_id) result.push_back(&widget);
    }
    return result;
}

void Canvas::layout_children(Widget& parent) {
    auto children = children_of(parent.id);
    if (children.empty() || parent.layout_mode == LayoutMode::Absolute) return;

    const float left = parent.rect.position.x - parent.rect.size.x * 0.5f + parent.padding.x;
    const float top = parent.rect.position.y - parent.rect.size.y * 0.5f + parent.padding.y;
    const float content_width = std::max(0.0f, parent.rect.size.x - parent.padding.x * 2.0f);
    const float content_height = std::max(0.0f, parent.rect.size.y - parent.padding.y * 2.0f);

    if (parent.layout_mode == LayoutMode::Row) {
        float cursor = left;
        for (auto* child : children) {
            child->rect.position = {
                cursor + child->rect.size.x * 0.5f,
                top + content_height * 0.5f
            };
            cursor += child->rect.size.x + parent.spacing;
        }
    } else if (parent.layout_mode == LayoutMode::Column) {
        float cursor = top;
        for (auto* child : children) {
            child->rect.position = {
                left + content_width * 0.5f,
                cursor + child->rect.size.y * 0.5f
            };
            cursor += child->rect.size.y + parent.spacing;
        }
    } else if (parent.layout_mode == LayoutMode::Grid) {
        const int columns = std::max(1, parent.columns);
        for (std::size_t i = 0; i < children.size(); ++i) {
            auto* child = children[i];
            const int column = static_cast<int>(i % static_cast<std::size_t>(columns));
            const int row = static_cast<int>(i / static_cast<std::size_t>(columns));
            const float x = left + child->rect.size.x * 0.5f +
                            column * (child->rect.size.x + parent.spacing);
            const float y = top + child->rect.size.y * 0.5f +
                            row * (child->rect.size.y + parent.spacing);
            child->rect.position = {x, y};
        }
    }

    for (auto* child : children) layout_children(*child);
}

void Canvas::layout(float width, float height) {
    for (auto& widget : widgets_) {
        if (!widget.parent.empty()) continue;

        const float half_w = widget.rect.size.x * 0.5f;
        const float half_h = widget.rect.size.y * 0.5f;
        switch (widget.anchor) {
            case Anchor::TopLeft:
                break;
            case Anchor::Top:
                widget.rect.position = {width * 0.5f, half_h};
                break;
            case Anchor::TopRight:
                widget.rect.position = {width - half_w, half_h};
                break;
            case Anchor::Left:
                widget.rect.position = {half_w, height * 0.5f};
                break;
            case Anchor::Center:
                widget.rect.position = {width * 0.5f, height * 0.5f};
                break;
            case Anchor::Right:
                widget.rect.position = {width - half_w, height * 0.5f};
                break;
            case Anchor::BottomLeft:
                widget.rect.position = {half_w, height - half_h};
                break;
            case Anchor::Bottom:
                widget.rect.position = {width * 0.5f, height - half_h};
                break;
            case Anchor::BottomRight:
                widget.rect.position = {width - half_w, height - half_h};
                break;
            case Anchor::Stretch:
                widget.rect.position = {width * 0.5f, height * 0.5f};
                widget.rect.size = {width, height};
                break;
        }
    }

    std::unordered_set<std::string> visited;
    for (auto& widget : widgets_) {
        if (widget.parent.empty() && visited.insert(widget.id).second) {
            layout_children(widget);
        }
    }
}

Widget* Canvas::hit_test(Vec2 point) {
    for (auto it = widgets_.rbegin(); it != widgets_.rend(); ++it) {
        auto& widget = *it;
        if (!widget.visible || !widget.enabled || !widget.interactable) continue;
        const float half_w = widget.rect.size.x * 0.5f;
        const float half_h = widget.rect.size.y * 0.5f;
        if (point.x >= widget.rect.position.x - half_w &&
            point.x <= widget.rect.position.x + half_w &&
            point.y >= widget.rect.position.y - half_h &&
            point.y <= widget.rect.position.y + half_h) {
            return &widget;
        }
    }
    return nullptr;
}

bool Canvas::pointer_down(Vec2 point) {
    auto* widget = hit_test(point);
    if (!widget) return false;
    if (widget->on_click) widget->on_click();
    return true;
}

} // namespace mju::ui
