# MJU Architecture

MJU is intentionally split into a small C++ runtime and platform/editor layers.

- **Core**: engine loop, scene, entities, transforms, math.
- **Render**: backend interfaces; first Android backend is OpenGL ES 2.0 for broad device coverage.
- **Input**: platform events will feed a platform-neutral event API.
- **IO**: scene/project serialization.
- **Platform**: Android lifecycle/JNI integration.
- **Editor**: future touch-first mobile editor over the same runtime.

The project avoids a dependency-heavy stack until a subsystem proves it needs one.


## v0.5 stability

The scene container reserves an initial 1024 entity slots so references returned by `create_entity` remain stable during normal editor-sized scenes.
