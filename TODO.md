# TODOS
- [x] Abstract and create a `camera.c`
- [x] Abstract and create a `texture_box.c`
- [x] Update `base.h`, `mate.h` and `mate.c` script
- [x] Copy DLLs into `build` folder since we have to do it manually, use `FileCopy`
- [x] Move to `Asserts`, move all to `Obj` 
- [x] Create PointLight, SpotLight and DirLight
- [x] Remove model loader
- [x] Better errors
- [x] Debug with `glDebugMessageCallback()`
- [ ] Make less GPU calls
    - [x] Use UBOS for view uniform, fog, night, etc.
    - [x] `UseShader` should check what shader we have bound at the moment, if same shader skip
    - [x] HashMap for uniforms instead of so many GL calls - tried, didn't change much UBOS are better
    - [ ] Batch rendering
- [ ] Add a system for importing inside GLSL, e.g. `#include "common/shadows.glsl"`, and it imports automatically from `./src/shaders/common/shadows.glsl`
    - [ ] shadows.glsl
    - [ ] fog.glsl
- [ ] Better GL abstractions, 
    - [x] Prefix with GL
    - [ ] SubData
    - [ ] Gen VBOs, EBOs, etc
- [ ] Face Culling
    - [ ] SkyBox
    - [ ] Implement WBOIT for order agnostic blending
    - [ ] `glEnable` culling for objects that benefit, disable for those who dont
- [ ] Optimal rendering, only render whats necessary

## base.h
- [ ] Simple HashMap implementation
