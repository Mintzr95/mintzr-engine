# MJU Engine architecture

MJU is not a fork of Godot, raylib, or another game engine. The target is a small, mobile-first C++ engine with its own runtime model, editor model, project format, and public API.

## Core layers

1. Platform layer
   - Android first.
   - SDL3 can provide low-level platform/window/input services.
   - Platform code is isolated behind MJU interfaces.

2. Render layer
   - MJU owns the renderer API and render-queue model.
   - Backends are replaceable. The first target is a lightweight OpenGL ES 2/3 path for weak phones, followed by a more capable backend.

3. World layer
   - Scene/entity/component data is MJU-owned.
   - EnTT is an optional storage/ECS primitive, not the public MJU object model.

4. Physics layer
   - MJU exposes its own physics API.
   - Box2D is an optional implementation backend.

5. Asset/resource layer
   - MJU project and asset metadata are independent from any external engine.
   - Importers will normalize textures, atlases, tilemaps, fonts, audio and animations into MJU resource types.

6. Gameplay layer
   - Input actions, timers, signals/events, animation state, UI, particles and scripting are MJU systems.
   - The public scripting surface must not expose Box2D/SDL/EnTT types directly.

7. Editor layer
   - Project browser, scene tree, inspector, viewport, tilemap editor, animation editor, asset browser and build/export controls.
   - Editor state is separate from runtime state so exported games do not carry editor dependencies.

8. Build/export layer
   - Desktop development build.
   - Android APK/AAB pipeline.
   - Later: Web export and additional native platforms.

## Speed strategy

We reuse mature, permissively licensed low-level components rather than rewriting commodity infrastructure. The engine architecture itself remains MJU-owned. This lets development focus on the distinctive editor, scene model, mobile runtime, asset pipeline and export workflow.

## Rules for imported source

Do not paste an entire external engine into MJU. Import a small, clearly identified subsystem only when there is a measurable benefit and the license is compatible. Prefer a build dependency when copying source would create unnecessary maintenance and attribution cost.

Every imported dependency is pinned to a known revision and documented in `THIRD_PARTY_NOTICES.md`.
