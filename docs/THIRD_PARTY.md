# MJU third-party components

MJU is a separate game engine. Third-party projects are used as replaceable building blocks and are not the MJU architecture.

## Current components

| Component | Purpose in MJU | License | Source |
|---|---|---|---|
| SDL3 | Mobile/desktop platform, input and lifecycle plumbing | zlib | https://github.com/libsdl-org/SDL |
| Box2D | Optional 2D rigid-body physics backend | MIT | https://github.com/erincatto/box2d |
| EnTT | Optional ECS/data-oriented backend | MIT | https://github.com/skypjack/entt |
| raylib 5.5 | Optional 2D reference/tooling backend; not the MJU public API | zlib/libpng | https://github.com/raysan5/raylib |
| Dear ImGui 1.92.9 | Desktop editor/debug tooling only | MIT | https://github.com/ocornut/imgui |
| miniaudio 0.11.25 | Replaceable audio backend | Public Domain or MIT-0 | https://github.com/mackron/miniaudio |

## Architecture rule

MJU owns these public concepts even when an implementation delegates to a third-party library:

- `mju::Engine`
- scenes and entities
- asset/resource handles
- renderer interface
- 2D camera and batching policy
- tilemap format
- animation data format
- runtime UI
- project format and editor project state
- scripting API
- Android application lifecycle integration

A third-party library must stay behind an MJU-owned adapter whenever practical. Game projects should depend on MJU APIs rather than directly depending on these libraries.

## Source reuse policy

We prefer linking/fetching upstream libraries as dependencies over copying large portions of an engine. When source is copied into the repository, the exact upstream version, path, license and required notices must be recorded here and in `THIRD_PARTY_NOTICES.md`.

## Mobile-first target

The runtime target is Android 2D first. Desktop editor features are separated from the runtime so Android builds do not carry editor-only dependencies such as Dear ImGui.
