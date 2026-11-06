# How EglantinePS5 wraps the core

What an agent needs before judging an upstream unit: which core code the PS5 build compiles, where the port already changes it, and what the console does differently from a desktop.

## The core and its changes

The core is the `Eglantine` submodule at `3140648a668aaebf87bae2dea17512fdca125ba0`. The build does not compile it as checked out. These layers change it, in this order:

| Layer | Where | What it does |
|---|---|---|
| Patches | `tools/eglantine-patches/0001`–`0017`, applied by `tools/apply-eglantine-patches.sh` | Applied to the submodule's working tree, in order. Later patches change lines earlier ones added, so the script finds how many are in before applying the rest; `--revert` takes them all out |
| Source copy | `tools/sync-eden-source.sh` | Copies the patched submodule into the build cache's source tree, stamped with its commit and a hash of the patches |
| Anchors | `tools/prepare-vulkan-port.py` | Rewrites 32 core files at build time by text replacement, using code from `headless/`: the Vulkan platform boundary (instance, library, device, swapchain, present), the scheduler, staging buffers, the memory allocator, the texture and buffer caches, `vk_rasterizer.cpp`, `vk_query_cache.cpp`, frame generation |
| CMake rewrites | `headless/CMakeLists.txt` (245 `string(REPLACE)` edits) and `headless/*.cmake` (`jit-alias`, `jit-page-table`, `scalar-memory`, `checked-fastmem`, `macro_native`, `macro_recovery`, `slot_vector_recovery`, `pipeline_wait`, `multiplayer_packets`) | Read about 100 core files at configure time and compile derived copies: dynarmic's x64 backend, the kernel (`k_page_table_base`, `k_memory_manager`, `k_thread`, `physical_core`), services (filesystem, nvdrv, nvnflinger, vi), `file_sys` (`vfs_real`, `patch_manager`, `ips_layer`, `fsmitm_romfsbuild`, `bis_factory`), loaders (`nso`, `deconstructed_rom_directory`), `common` (page tables, host memory, fiber, thread, logging), the OpenGL renderer, `gpu_thread`, macros, HID and input. A unit that changes one of these files can break a replacement silently or conflict with it: `rg -n '<path>' headless/CMakeLists.txt headless/*.cmake` |
| Frontend | `headless/` (its `CMakeLists.txt` and `inject.cmake`) | The PS5 app, `eden-headless`, links Eden's `core` target. `inject.cmake` adds it after the core's targets exist and puts two derived headers first on every include path: `common/sparse_large_vector.h` and the native macro headers |

`make update` (`tools/update-eglantine.sh`) reverts the patches, moves the submodule to the newest commit of its default branch, applies the patches again and runs `prepare-vulkan-port.py --report`, which names every anchor that no longer matches. On `v2` the submodule moves this way or to a chosen commit; each patch that stops applying is made again (`tools/make-core-patch.sh`) or dropped when upstream has the same change.

### The patches

| Patch | Files | Areas (files in each) |
|---|---|---|
| 0001 extracted versioned content | 1 | `core/loader` 1 |
| 0002 Ampere u16 F2I source modifiers | 2 | `shader_recompiler` 1, `tests` 1 |
| 0003 audio out manager session lifetime | 4 | `core/hle/service` 4 |
| 0004 Mario Kart performance and readback | 124 | `shader_recompiler` 31, `video_core` 20, `video_core/renderer_vulkan` 18, `tests` 13, `core/hle/service` 9, `video_core/texture_cache` 7, `video_core/vulkan_common` 7, `common` 4, `video_core/engines` 4, `video_core/query_cache` 4, `core` 3, `core/arm` 1, `core/memory` 1, `video_core/buffer_cache` 1, `video_core/textures` 1 |
| 0005 gate allocation and transcode dumps | 2 | `video_core/renderer_vulkan` 1, `video_core/vulkan_common` 1 |
| 0006 unwritten page reads and spin waits | 18 | `video_core` 4, `common` 3, `core` 2, `video_core/renderer_vulkan` 2, `core/hle/kernel` 1, `tests` 1, `video_core/buffer_cache` 1, `video_core/engines` 1, `video_core/query_cache` 1, `video_core/texture_cache` 1, `video_core/vulkan_common` 1 |
| 0007 texture cache costs and split diagnostics | 13 | `video_core` 4, `core/hle/kernel` 3, `video_core/renderer_vulkan` 2, `core/hle/service` 1, `video_core/engines` 1, `video_core/texture_cache` 1, `video_core/textures` 1 |
| 0008 Bananza terrain counters | 4 | `video_core` 3, `video_core/engines` 1 |
| 0009 software HMMA | 1 | `shader_recompiler` 1 |
| 0010 sparse zero clears | 3 | `video_core` 2, `core/hle/service` 1 |
| 0011 sparse VMA pool | 2 | `video_core/vulkan_common` 2 |
| 0012 collect emptiest blocks | 5 | `video_core/texture_cache` 2, `video_core/vulkan_common` 2, `video_core/renderer_vulkan` 1 |
| 0013 native Ampere HMMA | 6 | `shader_recompiler` 5, `video_core` 1 |
| 0014 pipeline statistics log | 2 | `video_core/renderer_vulkan` 1, `video_core/vulkan_common` 1 |
| 0015 NIR compute backend | 4 | `video_core/renderer_vulkan` 3, `shader_recompiler` 1 |
| 0016 integer add overflow | 2 | `shader_recompiler` 2 |
| 0017 compute qualification statistics | 1 | `video_core/renderer_vulkan` 1 |

The exact files of a patch: `rg '^\+\+\+ b/' tools/eglantine-patches/<patch>.patch`. Patch 0004 is the large one: besides the GPU core and the Vulkan renderer it adds task and mesh shader translation, UPOPC and ISBERD/ISBEWR to the recompiler, and readback tracking, the host command queue and GPU-only backing (`ReleaseBacking`) to the GPU core and services.

A unit that changes a file a patch or a CMake rewrite touches either already contains the change (`in port`, and the patch is dropped when the submodule moves past it) or conflicts with it (the patch or the replacement is made again on top). In the Why, a CMake rewrite is written `cmake: <file>`.

## Dependencies beside the repository

`make deps` (`tools/deps.py`, pins in `tools/deps.json`) fetches what is missing. Four are git checkouts next to the repository, which the build finds through `../`:

| Checkout | What it is |
|---|---|
| `../ps5-native-app-boilerplate` | Payload SDK v0.42, the runtime `libc.prx` and the native packaging tool |
| `../mihawk-vulkan-review` | Mihawk's PS5_Vulkan: the RADV build and link recipes; its `.deps/native` holds the built driver |
| `../mihawk-mesa-review` | Mihawk's PS5_Mesa: RADV's source |
| `../mihawk-sdk-review` | Mihawk's Payload SDK fork that RADV is built with |

A build from a worktree resolves `../` from the worktree's folder, so it needs these four next to it (links to the ones in `/Volumes/Hangar/ps5/`).

RADV is built once by `make prepare`, then isolated beside the OpenGL Mesa by `tools/isolate-radv.py`, which renames every implementation symbol in the x86-64 archive with `objcopy`. The build needs an x86-64 Linux host, as the upstream CI has: the isolation uses the host's GNU binutils, the link takes the host clang's x86-64 builtins, and the checks after the link compile x86-64-only port code for the host. On the Air it runs in an amd64 container under Rosetta (`PLAN.md`).

## What the PS5 changes

| Area | On the PS5 | What follows for triage |
|---|---|---|
| CPU | Zen 2, 8 cores; dynarmic's x86-64 backend | ARM64-backend changes do not apply; x86-64 backend and IR optimizations do |
| Guest memory | Fastmem is off for 64-bit games: every JIT load and store goes through Eden's page table (`docs/JIT-DIRECT-PAGE-TABLE.md`) | Changes to the page table, `core/memory.cpp` or the JIT's memory path are high value, and touch patch 0006 |
| Memory pool | One 12 GiB direct-memory pool shared by guest RAM, the page table and the GPU (`docs/MKW-MEMORY-BUDGET.md`) | Mario Kart World runs it out (`VK_ERROR_OUT_OF_DEVICE_MEMORY`); lower memory use and allocation churn are worth more than on a desktop |
| GPU | RDNA2 through Mihawk's RADV; OpenGL 4.6 through ps5-opengl as the fallback | Vendor workarounds for other drivers do not apply; check `vulkan_device.cpp`'s driver checks for RADV. Switch 2 ASTC is re-compressed to BC3 on this pool (`UseBc3ForSwitch2OnSmallCards`) |
| Display | The port's own present path (`headless/vulkan_surface.cpp`, `tools/patch-radv-wsi.py`), 60 or 120 Hz | Upstream changes to the swapchain, present manager or blit screen land on anchors |
| Frontend | `eden-headless` with its launcher (`headless/eglantineps5`), settings and input | Qt, Android, CLI and desktop settings UI do not apply |

## Measuring

`tools/mkw-bench-console.py` is the A/B runner (`docs/MKW-BENCHMARK.md`): it uploads each candidate, interleaves them (A B A B), warms the caches with one unmeasured run, measures windows of `EDEN_VULKAN_FRAME` after 800 presented frames, records device-memory peaks and crashes, and writes `results.json` and `report.md`. Candidates must be built with `EDEN_SCRIPTED_INPUT=ON`.

EglantinePS5's own baseline (2026-11-05, its PR #40 build, Vulkan, 0.5x, 1080p/60 Hz, warm caches): 23.5 FPS weighted with the JIT macros, out of device memory at 170 s. `main` is measured the same way before any `v2` result counts.
