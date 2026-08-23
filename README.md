# Oracyn — Custom Game Engine

A C++20 rendering engine built from scratch on OpenGL, focused on a solid, well-understood PBR pipeline and iterative polish.

### What's Working

- **Scene graph construction and traversal** — flat hierarchies, nested multi-level hierarchies, matrix-based and TRS/quaternion transforms all validated
- **Indexed geometry loading** — arbitrary buffer layouts (interleaved and separate), 16-bit and 32-bit index widening via cgltf
- **Full PBR material pipeline** — 5-texture slots (albedo, normal, metallic-roughness, occlusion, emissive) with correct sRGB/linear color-space handling
- **Khronos glTF 2.0 support** — via cgltf, with comprehensive asset-loading and validation
- **Real-world asset rendering** — AntiqueCamera and DamagedHelmet render correctly end-to-end

### Known Issues / Limitations

**Remaining low-priority issues:**
- Non-indexed primitives trigger null-pointer crash (fix identified, not yet applied)
- No bounds-checking on array access (materials, JSON parsing) — crash-prone on malformed input
- UBO buffers never freed (resource leak in destructors)
- Tangent attribute reads only 3 of 4 components in VAO setup

**Explicitly untested / unsupported:**
- Skinning / skeletal animation
- Morph targets
- Sparse accessors
- Multiple UV sets
- `.glb` binary container format
- Draco mesh compression
- Material extensions beyond standard PBR
- Alpha blend/mask modes


## Export Pipeline (Locked)

**Validated against:** Blender's official Khronos glTF 2.0 exporter → `.gltf` (separate files: `.gltf` + `.bin` + textures)

### Critical Blender Export Settings

| Setting | Value | Why |
|---|---|---|
| Format | glTF Separate (.gltf + .bin + textures) | Only variant tested; `.glb` untested |
| Apply Transform | ON | Without this, un-applied object transforms export as extra parent matrices, corrupting geometry positioning |
| Tangents | ON | Required for normal-mapping TBN calculation |
| Draco Compression | OFF | Not supported by loader |

### Texture Color Space

| Texture | Space | Engine Handling |
|---|---|---|
| Base Color / Albedo | sRGB | Gamma-decoded in shader (`pow(x, 2.2)`) |
| Normal / MR / Occlusion | Linear | Not decoded — read as-is |
| Emissive | sRGB | Gamma-decoded in shader |


## Build

### Setup

```bash
.\pre_build.bat    # Installs/verifies all tools and SDKs (runs automated)
.\build.bat        # Configures and builds with CMake/Ninja
```

Both scripts are comprehensive and include detailed logging. See comments in each for configuration options.

## Performance Targets

No profiling has been done. Current scene complexity is minimal (single model, single material, basic lighting).
- 60 FPS at 1920x1080 on moderate hardware
- Reasonable load times for assets (~1-2 seconds per model)

## Learning Goals

- Deep, correct PBR material pipelines
- glTF asset loading and validation at production quality
- Skeletal animation
- Cross-platform-minded build infrastructure (even while targeting Windows first)
- Disciplined OpenGL resource management (avoiding the implicit-state pitfalls found this session)

## License

Proprietary — internal project.

## Resources

- **Khronos glTF Specification:** https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html
- **PBR Theory:** https://www.khronos.org/blog/physically-based-rendering-in-filament
- **glTF Sample Models:** https://github.com/KhronosGroup/glTF-Sample-Models

## Contact / Notes

This is an active learning project. Architecture and design decisions are subject to change as the engine evolves.
