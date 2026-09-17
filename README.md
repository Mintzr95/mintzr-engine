# MJU Engine

Mobile-first C++20 2D game engine foundation.

Current version: **0.4.0**

## Current foundation

- C++20 core runtime with stable indexed entity lookup, bounded scenes, transform hierarchy and multi-scene management.
- 2D transforms, sprite batching, animation state, tilemaps and TMX import.
- Mobile GLES2 renderer with pixel-art filtering, optional mipmaps, missing-asset diagnostics, camera/tile culling and bounded GPU texture caching.
- Physics with dynamic/static/kinematic bodies, broad phase, AABB resolution, restitution, friction, contact callbacks and solid-tile collision.
- Pooled particle emission.
- Miniaudio-backed audio loading and playback.
- Canonical versioned JSON scene/project serialization with checksums and legacy migration paths.
- Bounded script VM with variables, arithmetic, conditions, loops, functions, events and physics/audio commands.
- Runtime UI with anchors plus row, column and grid container layouts with cycle-safe hierarchy traversal.
- Android JNI bridge where native C++ scene/editor state is authoritative; editor transform drags use one undo transaction.
- Standalone `mju_samples` target for demonstration content without hardcoding demo behavior into `Engine`.
- CMake + GitHub Actions Linux/Android build and regression-test pipeline, including a JNI contract check.

## Scope

MJU is a hardened development foundation, not a declared 1.0 release. The implemented renderer is GLES2. Production GLES3/Vulkan backends, GPU compressed texture upload/transcoding, and physical-device audio validation are explicitly not claimed as complete yet.

See `ENGINE_HARDENING.md` for the issue-by-issue closure report.
