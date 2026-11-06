# gpu-opengl

Units of the upstream delta whose largest share of changed lines is in the OpenGL renderer (the PS5 port's fallback) (`src/video_core/renderer_opengl/`). Writer: the agent assigned to this file; nobody else edits it. Verdicts and how to read a unit: [README](../README.md).

| Unit | Date | Title | Size | Also touches | Diff | Verdict | Why |
|---|---|---|---|---|---|---|---|
| dj#305 | 2026-11-03 | video_core: bump the shader cache versions for the FSEL lowering fix | +2 -2, 2 files, 1 commits | gpu-vulkan | `8c6539b564..5c8858ebdb` | in port | The pin's lower_to_maxwell.h already keeps FSEL's .NEG/.ABS (60ef6d2750) and its later 3140648a66 put CACHE_VERSION at 21 in gl_shader_cache.cpp and 25 in vk_pipeline_cache.cpp, past this bump's 16 and 19, so taking it would lower them; anchors: vk_pipeline_cache.cpp (patch 0015). |
