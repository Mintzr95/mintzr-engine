#include "mju/ui/ui.h"
#include <cassert>

int main() {
    mju::ui::Canvas canvas;
    auto& row = canvas.add("row");
    row.rect.size = {300, 100};
    row.rect.position = {160, 80};
    row.layout_mode = mju::ui::LayoutMode::Row;
    row.padding = {10, 10};
    row.spacing = 5;

    auto& first = canvas.add("first");
    first.parent = "row";
    first.rect.size = {50, 30};
    auto& second = canvas.add("second");
    second.parent = "row";
    second.rect.size = {70, 30};

    canvas.layout(320, 240);
    assert(first.rect.position.x < second.rect.position.x);
    assert(first.rect.position.y == second.rect.position.y);

    row.layout_mode = mju::ui::LayoutMode::Grid;
    row.columns = 2;
    canvas.layout(320, 240);
    assert(first.rect.position.x != second.rect.position.x);

    first.interactable = true;
    bool clicked = false;
    first.on_click = [&clicked]() { clicked = true; };
    assert(canvas.pointer_down(first.rect.position));
    assert(clicked);
    return 0;
}
