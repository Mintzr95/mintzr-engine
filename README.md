# MJU Engine

Mobile-first C++20 2D game engine foundation.

Current version: **0.3.0-pre**

## Current foundations

- C++20 core runtime with scene hierarchy, stable indexed entity lookup, limits and multi-scene management.
- 2D transforms, sprite batching, animation state, tilemaps and TMX import.
- Mobile GLES2 renderer with pixel-art filtering, optional mipmaps, diagnostics and bounded GPU texture caching.
- Physics with dynamic/static/kinematic bodies, AABB resolution, restitution, friction, contact callbacks and solid-tile collision.
- Pooled particle emission.
- Miniaudio-backed audio loading and playback.
- JSON scene/project serialization with version checks and legacy project migration.
- Bounded script VM with variables, arithmetic, conditions, loops, functions, events and physics/audio commands.
- Runtime UI with anchors plus row, column and grid container layout.
- Android JNI bridge where the native C++ scene/editor state is authoritative.
- CMake + GitHub Actions Linux/Android build and regression-test pipeline.

## Scope

MJU is still a development foundation, not a declared 1.0 release. The editor, asset pipeline, renderer backends beyond GLES2 and platform packaging are being expanded incrementally and are validated through the repository CI before release claims are made.
