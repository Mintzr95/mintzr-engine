# MJU Engine architecture

MJU is not a fork of Godot, raylib, or another game engine. The target is a small, mobile-first C++ engine with its own runtime model, editor model, project format, and public API.

## Core layers

1. Platform layer
   - Android first.
   - SDL3 provides optional low-level platform/window/input/lifecycle services.
   - Platform code is isolated behind MJU interfaces.

2. Render layer
   - MJU owns the renderer API and render-queue model.
   - The first mobile backend is a lightweight OpenGL ES 2/3 path.
   - The GLES2 renderer batches visible solid-color quads into one draw call per batch and exposes frame statistics for profiling.
   - Texture rendering will use the same MJU-owned command path rather than exposing raw backend objects to gameplay.

3. World layer
   - Scene/entity/component data is MJU-owned.
   - EnTT is an optional storage/ECS primitive, not the public MJU object model.
   - Entity hierarchy, transforms, sprites, visibility, layers and editor state stay in MJU types.

4. Physics layer
   - MJU exposes its own physics API.
   - Box2D is an optional implementation backend.

5. Asset/resource layer
   - MJU project and asset metadata are independent from any external engine.
   - stb_image provides lightweight RGBA decoding during import when enabled.
   - Importers normalize textures, atlases, tilemaps, fonts, audio and animations into MJU resource types.
   - Runtime resources are handle-oriented so the editor can unload/reload assets without exposing backend objects.

6. Serialization layer
   - nlohmann/json provides the parser/serializer implementation.
   - MJU owns the scene schema (`mju-scene`, versioned) and project format.
   - Scene save/load is covered by automated regression tests.

7. Gameplay layer
   - Input actions, timers, signals/events, animation state, UI, particles and scripting are MJU systems.
   - The public scripting surface must not expose Box2D/SDL/EnTT types directly.

8. Editor layer
   - Project browser, scene tree, inspector, viewport, tilemap editor, animation editor, asset browser and build/export controls.
   - Dear ImGui is limited to desktop tooling; exported Android runtime builds do not carry editor-only UI dependencies.
   - raylib is limited to optional development/reference tooling and is not the runtime API.

9. Audio layer
   - MJU owns audio buses and clip handles.
   - miniaudio is a replaceable low-level backend and supports Android.

10. Build/export layer
   - Desktop development build.
   - Android APK/AAB pipeline.
   - Later: Web export and additional native platforms.

## Mobile performance rules

- Avoid one draw call per sprite; batch compatible primitives.
- Prefer pooled allocations and stable resource handles in frame-critical code.
- Keep editor-only libraries out of Android runtime builds.
- Use compressed/decoded asset caches rather than repeatedly decoding files every frame.
- Keep physics, particles and animation updates bounded by active entities and avoid unnecessary work for hidden/inactive nodes.
- Add counters and regression tests before claiming an optimization is effective.

## Speed strategy

We reuse mature, permissively licensed low-level components rather than rewriting commodity infrastructure. The engine architecture itself remains MJU-owned. This lets development focus on the distinctive editor, scene model, mobile runtime, asset pipeline and export workflow.

## Rules for imported source

Do not paste an entire external engine into MJU. Import a small, clearly identified subsystem only when there is a measurable benefit and the license is compatible. Prefer a build dependency when copying source would create unnecessary maintenance and attribution cost.

Every dependency is recorded with its license and intended role in `THIRD_PARTY_NOTICES.md`. Before release, floating dependencies such as stb must be pinned to an exact upstream revision and recorded.
