# MJU Engine — third-party notices

MJU is a separate engine architecture. It may use permissively licensed third-party projects as replaceable low-level building blocks. Their licenses remain with their authors and required notices are preserved here.

## SDL3
- Project: https://github.com/libsdl-org/SDL
- License: zlib
- Purpose: platform/window/input/lifecycle plumbing.

## Box2D
- Project: https://github.com/erincatto/box2d
- License: MIT
- Purpose: optional 2D physics backend behind the MJU physics abstraction.

## EnTT
- Project: https://github.com/skypjack/entt
- License: MIT
- Purpose: optional ECS/storage primitive behind MJU-owned engine systems.

## raylib 5.5
- Project: https://github.com/raysan5/raylib
- License: zlib/libpng
- Purpose: optional low-level 2D reference/tooling backend. MJU does not copy raylib wholesale.

## Dear ImGui 1.92.9
- Project: https://github.com/ocornut/imgui
- License: MIT
- Purpose: desktop editor/debug tooling only; it is not part of the Android runtime UI.

## miniaudio 0.11.25
- Project: https://github.com/mackron/miniaudio
- License: Public Domain or MIT-0
- Purpose: replaceable audio backend with Android support.

## nlohmann/json 3.12.0
- Project: https://github.com/nlohmann/json
- License: MIT
- Purpose: project, scene, and editor serialization.

## stb
- Project: https://github.com/nothings/stb
- License: Public Domain or MIT
- Purpose: lightweight image decoding for texture import.
- Release policy: record the exact upstream revision before shipping; do not release MJU with an untracked floating revision.

## Godot Engine
- Project: https://github.com/godotengine/godot
- License: MIT
- Purpose: architectural/source-study reference only unless a specific file is deliberately imported after checking applicable third-party notices.
- Godot contains additional third-party components with their own licenses; this entry does not grant a blanket license for every embedded dependency.

## Import policy

Before copying source code from another repository, MJU must record the upstream repository, exact revision/tag, file or subsystem imported, applicable license, required copyright/attribution, and whether the code is copied, adapted, or merely used as a build dependency.

The preferred strategy is to fetch or link upstream libraries as replaceable dependencies and expose only MJU-owned APIs to games. Large portions of another game engine are not to be copied into MJU merely to change names or branding.
