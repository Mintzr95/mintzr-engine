# MJU Engine — third-party notices

MJU is a separate engine architecture. It may use permissively licensed third-party projects as replaceable low-level building blocks. Their licenses remain with their authors and required notices are preserved here.

## SDL3
- Project: https://github.com/libsdl-org/SDL
- License: zlib
- Purpose in MJU: platform/window/input/lifecycle plumbing.

## Box2D
- Project: https://github.com/erincatto/box2d
- License: MIT
- Purpose in MJU: optional 2D physics backend behind the MJU physics abstraction.

## EnTT
- Project: https://github.com/skypjack/entt
- License: MIT for code
- Purpose in MJU: optional ECS/storage primitive behind MJU-owned engine systems.

## raylib 5.5
- Project: https://github.com/raysan5/raylib
- License: zlib/libpng
- Purpose in MJU: optional low-level 2D reference/tooling backend. MJU does not copy raylib wholesale.
- Upstream release: 5.5.

## Dear ImGui 1.92.9
- Project: https://github.com/ocornut/imgui
- License: MIT
- Purpose in MJU: desktop editor/debug tooling only; it is not part of the Android runtime UI.
- Upstream release: 1.92.9.

## miniaudio 0.11.25
- Project: https://github.com/mackron/miniaudio
- License: Public Domain or MIT-0
- Purpose in MJU: replaceable audio backend with Android support.
- Upstream release: 0.11.25.

## Godot Engine
- Project: https://github.com/godotengine/godot
- License: MIT
- Purpose: architectural/source-study reference only unless a specific file is deliberately imported after checking its applicable third-party notices.
- Godot contains additional third-party components with their own licenses; this entry does not grant a blanket license for every embedded dependency.

## Import policy

Before copying source code from another repository, MJU must record the upstream repository, exact revision/tag, file or subsystem imported, applicable license, required copyright/attribution, and whether the code is copied, adapted, or merely used as a build dependency.

The preferred strategy is to fetch or link upstream libraries as replaceable dependencies and expose only MJU-owned APIs to games. Large portions of another game engine are not to be copied into MJU merely to change names or branding.
