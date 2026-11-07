# Findings across files

What the triage agents found that belongs to another subsystem's file, to the port itself or to the A/B plan. Writer: the main thread. The reviewer of each subsystem file applies the rows that name its file.

## For a subsystem file

| File | Unit | Finding | From |
|---|---|---|---|
| gpu-core | dj#260 | Its `texture_cache.h` eviction (costly images unused for 1800 frames) and the `FindBuffer` bound check (an MKW crash after 4.5 min) are gpu-core changes; leave out its `fiber.cpp` hunk, which would stop the CMake `FiberStack` replacement from matching | kernel-memory |
| gpu-core | li#22 | `CheckFeedbackLoop` skips repeated barriers unless depth or stencil was written: `measure` at most, correctness for colour feedback loops doubtful | kernel-memory |
| gpu-core | dj#535 | Sizing native storage buffers from the mapping for every native dispatch took Hyrule from 42-57 to about 2 FPS (dj#561); alone it should be limited to DLSS passes | gpu-core, on frontend-build's `port` |
| gpu-core | dj#218 | AV1 movies (Fire Emblem); plays only if the PS5 FFmpeg build adds libdav1d | frontend-build |
| gpu-core | li#59 | Must come with dj:660e5f0039 and dj:313463cd27 (gpu-vulkan): without it the back thread's idle hook submits while the front records (Kirby crash in `CommandChunk::Record`) | gpu-vulkan |
| gpu-core | dj:5dde7e9040 | A hang on stale command words in `gpu_split` / `dma_pusher` | shaders |
| gpu-core | li#150 | MME DMA reads skip pending query writes | shaders |
| gpu-core | li:62bc7538b1 | Root tables 0x504/0x508 | shaders |
| gpu-core | li:b3f983a6b8 | Bananza command-fetch ordering; its author marks it untested | shaders |
| gpu-core | dj#465 | Commit f130f0c50a lowers LDGSTS, TRAP and LDGDEPBAR in a general way, beyond what the skip covers; its FPTanh hunks matter to shaders | shaders, gpu-core |
| gpu-core | dj:cce6d44af2 | Changes `dma_pusher`; needed by dj:6612817cb4, dj:9fea7d8379, li:fae3bc64a9 and li#110 | services, gpu-core |
| gpu-core | dj#506 | Changes `kepler_compute` | services |
| gpu-core | li:45e4cfb18d, li#48 | Need dj#356 and dj:8604d2561a | services |
| gpu-core, gpu-vulkan, services, shaders | dj#337 | Fixes Pokopia stopping after its first frame (9f4f01f76e, 256b1dc346) and an MKW present stall (690f80e15f); each repairs a regression from a post-pin unit (dj:837133792d, dj:cce6d44af2, dj:0f14696044 and others) and travels with it | loader-fs |
| gpu-vulkan | li#50 | Mostly duplicates eglantine units, but its tessellation pipeline guard (6008b28f90) is no unit elsewhere | loader-fs |
| gpu-vulkan | li:d1b7f69df7 | Its `spv_module.reset()` duplicates dj:8551ee6462 | frontend-build |
| gpu-vulkan | dj:b69f7394f8 | The BC3 default changes only Switch 1 ASTC games; Switch 2 ones already get BC3 on the PS5 | kernel-memory |
| gpu-vulkan | dj:b00481fcc6 | A crash on RADV when `VK_EXT_mesh_shader` is missing, which matters for patch 0004's mesh pipelines | shaders |
| gpu-vulkan | li:62bc7538b1 | Quad control needs maximal reconvergence | shaders |
| gpu-vulkan | li#15 | A `ColorOutputType` NONE guard | services |
| gpu-vulkan | li:45e4cfb18d | A `FlushRegion` `InvalidateGPUCache` hunk | services |
| gpu-vulkan | li:f6a8f51324 | A 206-file squash judged `skip` as a whole, yet needed by li#77, li#75 (`nx2_sysmix.cpp`), li:4a58842d23 and li#144: split out what those need, or mark them `depends` on the part | cpu-jit, services, gpu-core |
| gpu-vulkan | dj:660e5f0039 | Its idle hook is needed by dj#639, li#59 and li#70 | gpu-core |
| gpu-vulkan | li#87 | Duplicates dj#567 (kernel-memory) and overlaps dj#561 / dj#571 (gpu-core); its shader lowering is shaders', and 20 of its 25 `lower_to_maxwell.h` hunks fail on the pin | gpu-vulkan |
| gpu-vulkan | li:8644f32fba | Carries dj#363 (gpu-core) and 22e0e41d6b from dj PR #330, which is only on an unmerged branch | gpu-vulkan |
| shaders | li#56 | The atomic-pointer carry and the `BlitConstantsPass` phi fix are no unit elsewhere | gpu-vulkan |
| shaders | li#71 | Its CFG translation speedup | gpu-vulkan |
| shaders | li:ca6b8e3ca0, li#47, li:8ff071d330, li:07a155d7b5, li:af0b66e57f, li#57, li#58 | Shader changes judged in frontend-build (li:07a155d7b5 has no effect on RADV) | frontend-build |
| shaders | dj:662545c5b0 | Holds only CI; its H0_NH1 fix is dj:77952c8d8c | frontend-build |
| shaders | dj#506 | Touches `shader_environment.h` and `lower_to_maxwell.h` | services |
| shaders | dj#464 | Adds a sixth leading count to the shader cache record; `headless/shader_cache_reader.inc` checks five, so the reader changes with it | shaders |
| shaders, gpu-vulkan | dj#305 | `in port`: the pin already has the FSEL fix and higher cache versions (GL 21, Vulkan 25); taking the bump would lower them | cpu-jit |
| services | li:b785035c5f | Changes `nx2_gsv.h` (Bananza present pacing, needs dj#270) | frontend-build |
| services | dj#270, li:62bc7538b1 | vi2 present pacing and `nvhost_as_gpu`; VI2 commands 104-107 and 354 (dj:6e20fc2a6e's `GetBuiltInFontData` is `in port`: the pin already takes the font type from the low byte) | shaders |
| services | li#56 | am command 1050 | gpu-vulkan |
| services | dj#393 | `skip` after review: it edits a StartThread wait the pin does not have (see kernel-memory) | kernel-memory, main thread |
| kernel-memory | dj:6e20fc2a6e | `in port` after shaders' review: the pin has its own StartThread handoff and the rest of what the PS5 runs of it | shaders |
| kernel-memory | li#150 | A 100 us idle spin in `physical_core.cpp` (patch 0007) | shaders |
| kernel-memory | dj:41a81ab5e3 | Needed by li#104 | gpu-core |
| loader-fs | dj#318, dj:8e777214b0 | Commit 8185d45d5d: loose Bananza dumps without `control.nacp` report 3.2.0 instead of 1.0.0 | frontend-build |
| loader-fs | dj#512 | Depends on dj#258 | kernel-memory |
| loader-fs | dj:5ae14067e3 | MKW 30 FPS / dynamic FPS modes need dj#334 (gpu-core); with it `measure`, and the PS5 needs its own setting for it | loader-fs |
| core | li#138 | A lock in `GPUDirtyMemoryManager::Gather`, which runs unlocked from every `InvalidateGPUCache` caller (judged `port` in frontend-build) | frontend-build |
| core | dj:76dd450322 | `skip` (WIP), but its `server_manager` change could be split out if a deferred-request hang appears | services |

## For the port

| Finding | From |
|---|---|
| `headless/CMakeLists.txt` switches `headless/dynarmic/reg_alloc.cmake` off because the pin's `reg_alloc.h` already has `IsReleased()`, so the port also loses its faster value lookup (`ValueLocation`) on the current pin | cpu-jit |
| `headless/main.cpp` registers no ExecuteProgram callback: a game that restarts itself would hang (li#138's eden-cli half) | frontend-build |
| dj:55274de6af adds a seventh `scheduler.Wait` in `vk_buffer_cache.cpp`, which breaks `prepare-vulkan-port.py`'s count of six wait sites | gpu-vulkan |
| Patch 0003 is in dj:34e1f691ae (PR #175): it is dropped once the submodule moves past it | cpu-jit |
| NUP, upstream's single-file package for extracted games, is `skip` as a core change; supporting it in the launcher is a feature decision | loader-fs |
| The pin's Ampere lowering tests (`ampere_f2i.cpp`, `ampere_kirby.cpp`, `ampere_integer_multiply.cpp`, `ampere_release_guards.cpp`, `ampere_release_f2i.cpp`, `ampere_uniform_lea.cpp`, run by `tools/check-nx2-core.py`) are in neither Djihads80's nor Lichii's branch, so no ported unit was run against them. On `horizon-v2` they refused 19 forms the units now lower: 18 on purpose (cc70030a81 updates the tests, unit by unit), and a TLD.SCR with bit 91, the uniform-handle flag, that 0xb66's path ignored (fixed in 9d6f094576). Found with a per-commit run of those tests over the 46 lowering commits (`/Volumes/Hangar/ps5/logs/test-bisect/`) | main thread |

## For the A/B

| Finding | From |
|---|---|
| `main` (pin, shader cache version 25) and `v2` would delete each other's caches in `tools/mkw-bench-console.py`'s shared, warmed cache. Resolved in `v2`: li#150 brought the versioned file names (`vulkan.fork-v28.bin`, `vulkan_pipelines.fork-v28.bin`), and a legacy file is adopted only at the same version, so `main`'s files stay as they are ([console-test.md](console-test.md#shader-caches)) | frontend-build, main thread |

## For delta.py

| Finding | From |
|---|---|
| dj:34f81e052c and dj:e6d53e728a repeat dj:c7dffce68a and dj:9a5a1fbdf6, which are already in the pin, on another base: units are told apart by SHA only, so a commit replayed on the other line (or into the pin) is listed again. A `git patch-id` check would mark them | cpu-jit |

`git patch-id --stable` over the 666 commits, against the pin's own commits since 2026-09-01 (main thread, 2026-11-05):

- Six units consist only of a commit whose patch the pin already has, and all six are `in port`: dj:0a13c9b2e0, dj:5c7d304cf5, dj:7f328339dd, dj:862ab88831, dj:920f3f6ebd, dj:bed068523e.
- Sixteen pairs of units carry the same patch, one commit replayed on another base. The pairs with matching verdicts: dj#267 / dj:b8d2c08d6e, dj#296 / dj#495, dj#297 / dj#495, dj:2ad0830d2f / dj:eb99da6072, dj:34f81e052c / dj:c7dffce68a, dj:4ec2e0c5de / dj:f0d1bbff79, dj:8739989d34 / dj:ad7b3b6c63, dj:9a5a1fbdf6 / dj:e6d53e728a, dj:cb4fbb73f8 / dj:f260bcb5a2, dj:e6996175ed / dj:f241ae67ba. In the six pairs whose verdicts differ, the shared commit must get one answer, and only one unit carries it to `v2`:

| Pair | Verdicts |
|---|---|
| dj#475 / dj:960a6a9ca4 | skip / measure (gpu-vulkan) |
| dj#561 / dj#571 | depends / skip (gpu-core) |
| dj:3d9a618c7c / dj:91691cf551 | measure / skip (gpu-core) |
| li:078b720a2f / li:be5bff1d3b | depends / skip (gpu-core) |
| li:4a58842d23 / li:7c27fdec8c | depends / skip (gpu-core) |
| li:ca6b8e3ca0 / li:e942bb7e87 | depends / skip (frontend-build) |

