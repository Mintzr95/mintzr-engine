# MJU Engine — Third-party notices

MJU is a separate engine architecture. It may use permissively licensed third-party projects as replaceable low-level building blocks. Their licenses remain with their authors and are reproduced/linked here so distribution can preserve the required notices.

## SDL3
- Project: https://github.com/libsdl-org/SDL
- License: zlib
- Purpose in MJU: platform/window/input/audio plumbing where enabled by CMake.
- Notice: SDL 2.0 and newer are distributed under the zlib license. See the upstream `LICENSE.txt` file.

## Box2D
- Project: https://github.com/erincatto/box2d
- License: MIT
- Purpose in MJU: optional 2D physics backend behind the MJU physics abstraction.
- Notice: preserve the Box2D license and copyright notice when distributing binaries containing Box2D.

## EnTT
- Project: https://github.com/skypjack/entt
- License: MIT
- Purpose in MJU: optional ECS/storage primitive behind MJU-owned engine systems.
- Notice: preserve the EnTT license and copyright notice when distributing binaries containing EnTT.

## Godot Engine
- Project: https://github.com/godotengine/godot
- License: MIT
- Purpose: architectural reference and source-study reference only unless a specific Godot file is deliberately imported after checking its applicable third-party notices.
- Notice: Godot contains additional third-party components with their own licenses. Do not assume the entire repository has a single license for every embedded dependency.

## raylib
- Project: https://github.com/raysan5/raylib
- License: zlib/libpng
- Purpose: implementation and API-design reference for lightweight rendering/input/resource ideas. MJU does not copy raylib wholesale.

## Policy

Before adding source code from another repository, MJU must record:
1. upstream repository and exact revision/tag;
2. file or subsystem imported;
3. applicable license;
4. required copyright/attribution text;
5. whether the code is copied, adapted, or merely used as a build dependency.

Permissive license compatibility does not remove the obligation to preserve notices. For code with more restrictive or incompatible licensing, MJU will not import it unless its licensing requirements are explicitly reviewed first.
