# MJU Engine hardening report

This document records the engineering follow-up against the original code-review report. It is intentionally conservative: a capability is marked complete only when it exists in the codebase and has a regression path in CI, or when the limitation is explicitly documented.

## Closed

### Performance and memory
- Scene lookup uses an indexed entity map instead of a linear search on every physics lookup.
- Entity storage is stable against `Entity&` invalidation from ordinary creation.
- Entity counts are bounded by a configurable scene limit.
- Entity IDs are monotonic and restored safely during serialization.
- Particle emission recycles slots instead of erasing live particles every frame.
- Renderer batches scene and tilemap work into one render/upload pass.
- Camera and tilemap visible-region culling reduce unnecessary submissions.
- GPU textures have a bounded byte budget and LRU-style eviction.

### Scene correctness
- Parent cycles are rejected for entity transforms.
- World transforms are computed through the hierarchy.
- Recursive deletion handles descendants and index cleanup.
- Duplication clones descendants.
- Parent references are validated during load.

### Physics
- Dynamic, static and kinematic bodies are represented.
- Broad-phase grid, overlap detection, penetration correction, restitution and friction are implemented.
- Contact callbacks exist.
- Solid tilemap collision is integrated with the physics step.

### Serialization and project files
- JSON is now the canonical scene format.
- Scene data includes animation state, hierarchy, IDs and a checksum.
- Scene versioning supports controlled migration.
- The old `MJU2` scene format remains readable as a migration path but is no longer the write format.
- Project files use versioned JSON and share one implementation between the editor and runtime project module.
- Legacy `MJU_PROJECT 1` project files remain readable.
- Project saves use a temporary file before replacement.

### Scripting
- Persistent per-entity variables exist.
- Arithmetic, conditions, loops, functions, events and calls exist.
- Physics and audio commands are exposed.
- Operation, loop and call-depth limits prevent unbounded execution.
- Event execution is isolated from the script top-level body.

### UI and editor
- Absolute, row, column and grid layouts exist.
- Parent hierarchy traversal is cycle-safe.
- Android editor transform drags are transaction-based, so a drag produces one undo checkpoint instead of one per MOVE event.
- Native C++ state remains authoritative for the Android editor.
- Zoom range is bounded to a mobile-usable range.

### Android/JNI and CI
- Main/editor JNI exports are contract-checked by a CTest Python test.
- Android lifecycle and orientation configuration are explicit.
- Android uses one native renderer path for scene + tilemap rendering.
- Linux CI configures, builds and runs the regression suite.
- Android CI builds and verifies a debug APK and uploads it as a workflow artifact.

### Dependencies and architecture
- Unused ghost dependencies were removed from CMake.
- The core runtime no longer injects a demo scene, demo button or demo particles during engine initialization.
- Demo content is available as the separate `mju_samples` target.
- Resource packs validate relative paths and reject traversal-style entries.

## Intentionally scoped for a later renderer milestone

These are not falsely marked as implemented:

- A production GLES3 renderer backend.
- A Vulkan renderer backend.
- GPU compressed texture upload/transcoding such as ETC2/ASTC/KTX2.
- Device-level audio playback validation on physical Android hardware.

The current shipping renderer remains GLES2 because it is the backend actually implemented and tested by the Android build. The architecture does not claim that an unimplemented Vulkan/GLES3 backend already exists.

## Current release posture

MJU remains a development foundation rather than a 1.0 release. The repository is intended to be buildable, testable and extensible without pretending that the future rendering backends or hardware-validation matrix already exist.
