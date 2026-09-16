# MJU Engine — Release Candidate

## What is included

- Independent C++20 engine architecture owned by MJU.
- Mobile-first GLES2 2D renderer with camera culling, VBO/IBO batching, texture caching, UV regions, sprite sheets and multi-texture batch ranges.
- TileMap runtime with versioned serialization, tileset metadata, visible-cell culling and GPU rendering.
- Sprite animation timing plus reusable animation clips/player API.
- Scene JSON v2 with backward-compatible loading for v1.
- Box2D, EnTT, SDL3, miniaudio, nlohmann/json, stb_image and tmxlite integrated as replaceable external building blocks where enabled.
- Android debug APK workflow using the pinned Android SDK/NDK/CMake versions from the Gradle project.
- Touch input and a mobile editor surface with hierarchy, inspector, undo/redo, scene saving and play mode.
- Mobile performance presets for low-end, balanced and high-end devices.
- Automated core, scene serialization, sprite batching, animation and TileMap regression tests.

## Release boundary

This branch is a release candidate, not a claim of bug-free publishing. The GitHub Actions build is the authoritative build check. A Play Store release additionally requires an Android signing keystore, release signing configuration and device testing on the target hardware.

## Third-party policy

MJU does not copy an entire existing engine. External projects are consumed behind MJU-owned APIs and are documented in `THIRD_PARTY_NOTICES.md`. Godot and other engines may be used as architectural references, but large subsystems are not wholesale copied into MJU.

## Mobile priorities

The renderer keeps transient geometry bounded, culls off-screen sprites and TileMap cells, caches decoded and uploaded textures, and provides explicit device-tier presets so low-end Android hardware can use lower texture, particle and batch budgets.
