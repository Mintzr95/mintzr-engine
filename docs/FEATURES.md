# MJU Feature Set

MJU is intentionally small to use but structured like a real engine.

Core: C++20 runtime, entities, transforms, scene persistence, camera, animation, collision.
Editor: hierarchy, inspector model, project files, play mode, undo/redo.
Assets: recursive catalog for textures, audio, fonts, scenes, scripts and data.
2D: tilemap data model and sprite-oriented entities.
UI: mobile-friendly widget model with anchors.
Android: NDK/CMake native core and Gradle packaging project.

The missing production-facing pieces are the visual renderer/editor interactions, full audio/physics backends, scripting runtime and a verified Android release pipeline.
