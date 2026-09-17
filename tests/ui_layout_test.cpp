#include "mju/ui/ui.h"
#include "test_harness.h"

int main() {
    mju::test::Runner t;
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
    MJU_EXPECT(t, first.rect.position.x < second.rect.position.x);
    MJU_EXPECT(t, first.rect.position.y == second.rect.position.y);

    row.layout_mode = mju::ui::LayoutMode::Grid;
    row.columns = 2;
    canvas.layout(320, 240);
    MJU_EXPECT(t, first.rect.position.x != second.rect.position.x);

    first.interactable = true;
    bool clicked = false;
    first.on_click = [&clicked]() { clicked = true; };
    MJU_EXPECT(t, canvas.pointer_down(first.rect.position));
    MJU_EXPECT(t, clicked);

    mju::ui::Canvas cyclic;
    auto& a = cyclic.add("a");
    auto& b = cyclic.add("b");
    a.layout_mode = mju::ui::LayoutMode::Column;
    b.layout_mode = mju::ui::LayoutMode::Column;
    a.parent = "b";
    b.parent = "a";
    cyclic.layout(320, 240);
    MJU_EXPECT(t, true);

    return t.finish();
}
