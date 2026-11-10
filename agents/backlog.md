# Backlog

What the upstream sync of 2026-11-10 (stage 7 of [PLAN.md](../PLAN.md#upstream-sync-2026-11-10)) left out, and what each item could bring. "Triage note" is the note written when the unit was triaged, copied from [delta/](delta/) or [prs/](prs/); figures in it are upstream's or EglantinePS5's own measurements unless it says otherwise. Commit hashes are `v2`'s.

## Left out while porting

Units the batches S1a, S1c, S1b and S3 were meant to take, or took in part.

### li#878: vulkan: free compute IR before a synchronous driver compile (precache memory)

- Source: [delta/gpu-core.md](delta/gpu-core.md), +75 -2, 4 files, 1 commits, verdict `port`.
- Triage note: CreateComputePipeline drops the program's IR, its SPIR-V vector and the block and instruction pools before a driver compile that runs on the calling thread (boot workers held up to hundreds of MiB for one large shader through the compile); v2 has the same code (vk_pipeline_cache.cpp:1738 and 1775) and takes that memory from the 12 GiB pool it shares with the guest; a hand edit of two hunks plus ObjectPool::Free; conflicts: vk_pipeline_cache.cpp, tests/CMakeLists.txt; anchors: vk_pipeline_cache.cpp.
- Why it is out: Dropped from batch S1c. It does not apply to `v2`'s pipeline cache: the hand port had to free the IR on the NIR compute path too, and nothing measures the gain.
- What it takes: The hand port exists as commit `0e6b84d9e9` on branch `s1c` of `/Volumes/Hangar/ps5/scratch/triage-20261110/work` (a scratch clone, kept only until stage 7 ends). Measure the pool during a shader precache with and without it before taking it.

### Core patch 0031 macro-state-writes (EglantinePS5 #60)

- Source: [prs/eglantineps5-main.md](prs/eglantineps5-main.md), +25 -1, 3 files, verdict `port`.
- What it does: A register write a macro makes takes `Maxwell3D::MacroWrite`: shadow RAM, same-value skip and dirty flags without `CallMethod`'s bookkeeping, unless profiling, tracing or front macros
- Applies to `v2`: clean. cmake: `maxwell_3d.cpp`, `macro.cpp`
- Triage note: Immediate, with `CallMethod`'s exact effect for state registers (compare `v2`'s `maxwell_3d.cpp:472-524`), so it is not the sink path `v2` rejected. Bananza's front runs 600 k macro calls and 5.3 M 3D writes a second. No switch and no isolated number; the `macro_native.cmake` anchor must change in the same commit or the build stops
- Why it is out: Dropped after the independent review: its fast path needs `!front_macros` and the console runs front macros, so on `v2` it does nothing.
- What it takes: Only worth it if front macros are turned off (`EDEN_NX2_MME_JIT=0`), which nobody plans. It needs the `headless/macro_native.cmake` anchor edited in the same commit.

### Core patch 0018 texture-cache-feedback-copies (EglantinePS5 #50)

- Source: [prs/eglantineps5-main.md](prs/eglantineps5-main.md), +190 -4, 9 files, verdict `adapt`.
- What it does: A draw that samples an alias of its own colour target copies the target once per guest texture barrier and render-target set, not once per draw (`EDEN_NX2_FEEDBACK_COPIES=exact` restores); alias walks only when an alias changed; overlap walks once per new overlap; vertex buffer sync skipped when no CPU mark touched the range
- Applies to `v2`: conflict: `texture_cache/image_base.h`, `texture_cache/texture_cache.h` (3 regions). anchors: vkport: `buffer_cache.h`, `buffer_cache_base.h`, `texture_cache.h`; cmake: `maxwell_3d.cpp`, `buffer_cache_base.h`
- Triage note: Bananza, Ingot Isle gate: 9.3 to 15.0 FPS (image copies 17,600/s to 2,600/s); Mario Kart World unchanged. The three conflicts are the alias-walk skip, which `v2` already has as `alias_modification_tick` (batch 2, fork `ad6dd1f179`): keep `v2`'s tick and take the feedback coalescing (`texture_barrier_epoch.h`, `feedback_*` fields) and the `overlap_epoch` walk. A deferred feedback copy must leave the image "older" so the next barrier copies it: with `v2`'s tick test that already holds. The vertex sync cache overlaps `v2`'s dj#562 (`HasPendingCpuUpload`, step 41): optional. `v2`'s `ReplaceColorFeedbackViews` (fork `a49c810637`) is a different mechanism (same-image feedback) and stays
- Why it is out: Taken in part (`b966a80`): the feedback copy coalescing and the `overlap_epoch` walk. `alias_maybe_newer` and the vertex sync cache are left out because `v2`'s modification tick and `HasPendingCpuUpload` already cover them.
- What it takes: Nothing expected. Listed so the missing hunks are not mistaken for an oversight.

### li#353: vulkan: don't write render target components the fragment shader does…

- Source: [delta/gpu-vulkan.md](delta/gpu-vulkan.md), +49 -12, 7 files, 1 commits, verdict `port`.
- Triage note: Take the scheduler.InvalidateState() after the bare-view BlitColor (blit_image.cpp, merges clean): the blit leaves dynamic state the tracker still thinks is bound, on every driver; leave out the colour write-mask half, which is for AMD's proprietary driver writing components the fragment shader does not output (RADV already masks by the shader's outputs, cb_shader_mask in radv_shader_info.c:896); conflicts in vk_graphics_pipeline.cpp and .h, vk_rasterizer.cpp and .h (all in the half left out); anchors: vk_rasterizer.cpp; cmake: vk_graphics_pipeline.h.
- Why it is out: Taken in part (`60b9452`): only `InvalidateState()` after the bare-view blit. The colour write-mask half is left out, as its triage note says.
- What it takes: See the triage note for what the write-mask half would need on `v2`.

### li#600: audio: return the registered managers from nx2:aud:u, add IAudioRenderer cmd 14

- Source: [delta/services.md](delta/services.md), +30 -7, 3 files, 1 commits, verdict `port`.
- Triage note: Only its IAudioRenderer command 14 (SetAsyncMemoryPoolControlEnabled, 23.0.0+, a logged no-op in audio_renderer.cpp and .h, which merge clean) is new; the shared audout:u and audren:u managers are in port, since v2's INx2AudioUserClient is built with the registered instances (the pin's form, patch 0003); conflict: nx2_audio_user_client.h.
- Why it is out: Taken in part (`961ce23`): `IAudioRenderer` command 14 only. `nx2_audio_user_client.h` is unchanged.
- What it takes: Take the manager half only if a game logs an unimplemented `nx2:aud:u` call.

### li#727: nx2: stub vi2 command 201 and nifm command 101 (#653)

- Source: [delta/services.md](delta/services.md), +3 -1, 2 files, 1 commits, verdict `port`.
- Triage note: Only the nifm IGeneralService command 101 stub (one line in nifm.cpp, called at boot by newer SDKs, Ocarina of Time among them) is new; nx2:vi2 201 is in port as RegisterClientState (nx2_gsv.h:1406), which is why nx2_gsv.h conflicts.
- Why it is out: Taken in part (`ab242d2`): the nifm `IGeneralService` 101 stub. `nx2:vi2` 201 stays `RegisterClientState` on `v2`.
- What it takes: Nothing unless a session logs vi2 201 failing.

### li#659: nx2: keep movie video in sync with its audio (imports #571, ports upstream 1f52ac7e2e)

- Source: [delta/services.md](delta/services.md), +211 -19, 12 files, 1 commits, verdict `port`.
- Triage note: Take two of its five commits: IStorage::Read drops the 40 MB/s Switch 1 card throttle for Switch 2 titles (fs_i_storage.cpp; Ocarina streams its movie with 300 to 460 reads a second, and every Switch 2 game loads through it), and AM serves nx2:gsv's sessions on three host threads with locks in hle_ipc.h and the applet services, which needs an A/B (cmake: server_manager.cpp, fs_i_storage.cpp, whose anchors stay); the late-frame drop is dj:1f52ac7e2e, and the queue-full refusal needs li#49's Nx2VideoInputQueue (skip), which v2 lacks; conflict: nx2_gsv.h.
- Why it is out: Taken in part (`e27508b`): the fsp half (no read throttle for Switch 2 titles). The movie half (decoder queue refusal on `Nx2VideoInputQueue`, `CoreTiming::GetPauseEndTime`) is not in `v2`.
- What it takes: The movie half goes with dj:1f52ac7e2e and li#809 below.

### EglantinePS5 #90: MKW without PS5 Pro memory (#86, #87 inside; content of #83, #84, #88)

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `port`.
- Files outside the core: `src/guest_memory_service.inc`, `src/memory_pages.cpp`, `headless/heap_arenas.inc`, `heap_pages.cpp`, `main.cpp`, `performance.cpp`, `nfs/nfs.cpp`, `CMakeLists.txt`, `tools/patch-radv-trim.py`, `check-heap-growth.py`, `check-jit-allocator.py`, `docs/MKW-MEMORY-BUDGET.md`, `docs/MKW-MEMORY-LEAK.md`
- Applies to `v2`: clean after #68 and #69
- Triage note: The only work measured on the owner's kind of console (12,224 MiB): Mario Kart World went from 0 of 3 runs reaching a race to 3 of 7, later two whole 500 s runs. Parts: file descriptor cap (`MESA_DISK_CACHE_DATABASE_NUM_PARTS=4`, a `setenv` in `main.cpp`; the console gives a process 267 descriptors); RADV trim quarantine (`RADV_PS5_TRIM_QUARANTINE_MS`, 2000); heap arena release and page-out; packing of GPU-reachable slots (`EDEN_CRAM_PACK_GPU=0` off) from 3 GiB largest free block and 30 s cold; JIT code cache steps taken as 2 MiB pieces; `EDEN_GUEST_FREE_ZERO`. Risks: packing was a GPU hang cause upstream before #88, so add this group only after `v2`'s own hang is understood; the JIT code cache change is checked by `check-jit-allocator.py`, which our Rosetta build skips. Section 6
- Why it is out: Taken in part (`7e115e3`): only `MESA_DISK_CACHE_DATABASE_NUM_PARTS=4` in `headless/main.cpp`. Core patches 0055 and 0058 came with it, but 0055's descriptor census has no caller on `v2`, and 0058 alone only frees shader modules into the heap: the pool gain needs this pull request's heap arena release.
- What it takes: Needs #68 (on `v2` as `7c9a68c`) and #69 first. See the Mario Kart World memory stack below.

## Mario Kart World, if the stall remains

The offline analysis ([mkw-hang-20261110.md](mkw-hang-20261110.md)) ranks the unwritten task count first, and `1e26c76` and `ecd174c` fix it. These are the next changes to try, one per boot, if the diagnostic boot still stalls, and the Mario Kart World work that only makes sense once it runs.

### Core patch 0056 index-buffer-span-cap (EglantinePS5 #90 (from #83))

- Source: [prs/eglantineps5-main.md](prs/eglantineps5-main.md), +37 -2, 2 files, verdict `adapt`.
- What it does: Index buffer span worked out in 64 bits, an index limit below the start means no bound, bindings capped at 128 MiB; an indexed draw past the cap is dropped
- Applies to `v2`: `buffer_cache.h` merges clean; conflict: `vk_rasterizer.cpp` (1 region). vkport: `buffer_cache.h`, `vk_rasterizer.cpp`
- Triage note: Mario Kart World's race load sends draws of 1.1-2.9 billion indices: the wrapped 32-bit span became a 2.9 GB index buffer (out of memory on the spot) and, drawn, held the GPU past the driver's 10 s limit (device lost). Every run that reached race load ended within 131-211 s; with it 4 of 4 ran 240 s. `v2` already drops such draws before they reach the buffer cache (li#142, `3e77e72dcc`: more than 2^24 vertices), which is the conflict: keep `v2`'s guard and take the `UpdateIndexBuffer` half as defence in depth
- Why it is out: Not taken. `v2` already drops direct draws of more than 2^24 vertices (`vk_rasterizer.cpp`), which is stronger than the patch's `Draw` half. Indexed indirect draws have no cap at all.
- What it takes: Take only the `UpdateIndexBuffer` half (64-bit span, 128 MiB cap). One conflict region in `vk_rasterizer.cpp`; keep `v2`'s guard.

### Core patch 0022 dense-lazy-gpu-regions (EglantinePS5 #67)

- Source: [prs/eglantineps5-main.md](prs/eglantineps5-main.md), +16 -1, 1 file, verdict `adapt`.
- What it does: nx2:gpu 214 GPU-only regions of 16 MiB or more are backed as the GPU writes them, like sparse ones (`EDEN_NX2_DENSE_LAZY_MIB=<n>`, 0 backs them whole)
- Applies to `v2`: clean
- Triage note: Mario Kart World: regions backed whole 8,830 MiB to 31 MiB of the guest kernel's system pool, and a 1,453 MiB region that got no backing now maps; the out-of-memory time did not move (163/173 s against 171/113 s). On `v2` it defeats batch 1's `InGpuOnlyRegion` (fork `9e19e33eba`): a lazily backed region is in `m_sparse_regions`, not `m_gpu_only_regions`, so Bananza's 216 map of one 200 MB block over each of its six regions would be applied again (the tiles over its video). Extend `InGpuOnlyRegion` to lazily backed dense regions first
- Why it is out: Not taken. On `v2` it defeats `InGpuOnlyRegion`: a lazily backed region lands in `m_sparse_regions`, so DK Bananza's map of one 200 MB block over each of its six regions would be applied again.
- What it takes: Mark lazily backed dense regions so `InGpuOnlyRegion` stays true for them, then take the patch with `EDEN_NX2_DENSE_LAZY_MIB=0` restoring the old backing.

### dj:ff72fed972: vulkan: mesh and task stages now require the guest warp size

- Source: [delta/gpu-vulkan.md](delta/gpu-vulkan.md), +12 -2, 1 files, 1 commits, verdict `port`.
- Triage note: Mesh and task stages that use warp (subgroup) ops get requiredSubgroupSize 32 like compute already does; the PS5 RADV fork lists task and mesh in requiredSubgroupSizeStages and picks wave64 for workgroups over 32 threads (radv_shader.c:361-425), so these stages can run at the wrong subgroup size in Mario Kart World and Ocarina; one hunk in vk_graphics_pipeline.cpp, merges clean; low priority.
- Why it is out: Not taken yet: step 3 of batch S2, one change per boot.
- What it takes: One hunk in `vk_graphics_pipeline.cpp`, merges clean. It changes how task and mesh pipelines are created, so it needs its own boot.

### dj#774: shader: lower BRA.U and HSETP2 with a constant buffer

- Source: [delta/shaders.md](delta/shaders.md), +16 -8, 3 files, 2 commits, verdict `port`.
- Triage note: HSETP2 with a constant-buffer operand (`0x634`, Mario Kart World `HSETP2.GT.AND P1, PT, RZ.H0_H0, c[0x7][0xfc].H1_H1, !P1`, "sometimes invisible roads") has no case in v2, which handles only `0x234` and `0x434`; its BRA.U half is already in v2 (batch 1). Take commit 1e2c2dd4db only (the range also carries README Discord links); conflict `lower_to_maxwell.h`; no ps5 patch counterpart; new form only, no cache bump.
- Why it is out: Not taken yet: step 3 of batch S2.
- What it takes: Commit `1e2c2dd4db` only; one conflict in `lower_to_maxwell.h`. A new lowering form, so no cache version bump.

### EglantinePS5 #77: RADV robustness, AV1 movies, overlay fields, haptics, themes, native MME default (#79, #80 inside)

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `adapt`.
- Files outside the core: `tools/patch-radv-gs-compute.py`, `check-radv-robustness.py`, `check-hud-mask.py`, `check-launcher-theme.py` (new), `build-radv-dependencies.sh`, `prepare-build.sh`, `build-headless-ffmpeg.sh`, `deps.json` (dav1d), `prepare-vulkan-port.py`, `mkw-bench-console.py`, `headless/pad_haptics.cpp`/`.h`, `hud_gpu_time.h` (new), `pad.cpp`, `devices.h`, `hud.h`, `vulkan_hud*.inc`/`.frag`, `macro_native_hook.h`, `settings_store.h`, `main.cpp`, `performance.cpp`, 20 launcher files under `headless/eglantineps5/` with `pe/ui/theme.cpp` (new), 29 `.po` catalogs, `docs/THEMES.md`, `THIRD_PARTY_NOTICES.md`
- Applies to `v2`: conflict: `headless/devices.h`, `headless/pad.cpp` (commit `f836d49`). The other 11 commits are clean in order
- Triage note: Eleven separable commits; by part: (1) `patch-radv-gs-compute.py`: port. The driver's geometry-as-compute passes no longer read and write through null pointers for a dropped draw, which faulted the GPU; one RADV rebuild (`EDEN_GS_COMPUTE_SHA256`). The transfer split is not to be taken: 9 GPU hangs in about 30 Mario Kart World runs against 0 in about 200, the same "GPU has not reached submission" message `v2` saw. (2) AV1 through dav1d (`9fdc65e`, `f733a61`): adapt with 0038; a new build dependency (owner's approval), Ocarina of Time's movies are AV1. (3) FPS overlay fields, size and corner (`a7fe054`): port. (4) Haptics (`f836d49`): `v2` wins for the amplitude and the 2 s stop (`aa1faa2`: the owner measured Eden's curve shaking nonstop in Bananza's cutscenes); what `v2` lacks is the DualSense audio-haptics route (`pad_haptics.*`) and the vibration strength setting. (5) Launcher themes (`b46539f`): port, launcher only. (6) Native MME macros on by default (`215ef6e`, `EDEN_NX2_MME_NATIVE=0` off, `=check` compares): measure; it only replaces macros whose SHA-256 matches a signed Mario Kart World layout, and "FPS no worse than the JIT". (7) `mkw-bench-console.py --events`: port. Measured for the whole pull request, before the transfer split was dropped: Mario Kart World 29.28 / 30.28 against 25.15 / 23.58 FPS; Bananza 12 starts, no crash
- Why it is out: Not taken. Eleven separable commits; the one that matters for the stall is `tools/patch-radv-gs-compute.py` (part 1). Its overlay fields, haptics setting and themes belonged to batch S5. Its transfer split must not be taken (see the note).
- What it takes: Part 1 needs one RADV rebuild (`EDEN_GS_COMPUTE_SHA256`). The launcher parts conflict in `headless/devices.h` and `headless/pad.cpp` (commit `f836d49`).

### RADV task ring zeroed (this port's own patch)

- What it is: RADV's task shader emulation writes each task's payload into a ring buffer it never clears, and nothing clamps the mesh workgroup count read back from it. Zeroing the ring makes an unwritten count read as zero in the driver too, whatever the guest shader does.
- Why it is out: Optional third fix of the stall analysis, not applied: `1e26c76` already writes the count in the shader.
- What it takes: The diff is `/Volumes/Hangar/ps5/scratch/triage-20261110/mkw/fix-3-radv-task-ring-zero.diff` (scratch, temporary). It needs a RADV rebuild.

### li#690: video_core: keep karts on the water and restore waves (Mario Kart World)

- Source: [delta/gpu-core.md](delta/gpu-core.md), +111 -3, 7 files, 1 commits, verdict `port`.
- Triage note: Mario Kart World water: a wave displacement fallback for empty depth inputs (ApplyMkwZeroDepthFallback in the new mkw_wave_workaround.h, source program key 0x5a4f3726590efb17, three instructions inserted at source offset 0x2d0) and the write-back of the 256x256 collision height map of compute program 0x6e0ae200ee9aa220 on the preemptive download path, so karts stay on the water and waves return; v2 has NativeProgramKey and source_instruction_offsets but LoweredNativeCode lacks the native_program_key field and CurrentComputePipeline the key output; a no-op if the owner's MKW version has other shader bytes; test once the device-lost hang allows a race; conflicts: vk_pipeline_cache.cpp, vk_pipeline_cache.h, vk_rasterizer.cpp; anchors: vk_pipeline_cache.cpp, vk_rasterizer.cpp, texture_cache.h; cmake: shader_environment.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10. It only matters once the game reaches a race.
- What it takes: See the triage note; it keys on two program hashes of the game.

### li#1079: texture_cache: skip a join's upload when layer copies overwrite it whole

- Source: [delta/gpu-core.md](delta/gpu-core.md), +81 -18, 4 files, 1 commits, verdict `depends`.
- Triage note: JoinImages also skips the upload when several layer copies together overwrite the new image: Mario Kart World renders each layer of its shadow map and colour arrays as its own target and then samples the array, 13-31 ms of upload per array; needs li#817's CopiesWriteWholeImage (port, ps5 patch 0061, which accepts only one whole copy), then port; conflicts: texture_cache.h, util.cpp, util.h; anchors: texture_cache.h; cmake: util.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: 0061 is on `v2` now (`2d40a05`) and accepts only one whole copy; this extends it to several layer copies. Conflicts in `texture_cache.h`, `util.cpp`, `util.h`.

### EglantinePS5 #69: MKW memory: cold guest memory packing, RADV 2 MiB pieces, driver trim

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `adapt`.
- Files outside the core: `src/guest_memory_service.inc`, `src/memory_pages.cpp`, `headless/heap_arenas.inc`, `heap_pages.cpp`, `main.cpp`, `performance.cpp`/`.h`, `tools/patch-radv-pieces.py`, `tools/patch-radv-trim.py` (new), `build-radv-dependencies.sh`, `prepare-build.sh`, `docs/MKW-MEMORY-BUDGET.md`
- Applies to `v2`: conflict: `headless/performance.cpp` (commit `5189f54`): keep `v2`'s 32 MiB gate on `eden_heap_sites_report` (`bb480a5`) and add the `EDEN_HEAP_SLACK` line
- Triage note: Stacked on #68. Cold guest slots are packed with LZ4 (27% of their size) once the pool's largest free block is short; RADV takes buffers from 2 MiB pieces and gives unused granules back; the heap reuses its emptiest arena. Mario Kart World's out-of-memory moved from 113-180 s to 240-360 s. Packing is on since this pull request (`EDEN_CRAM_PACK=0` turns it off) and only acts under pool pressure, which `v2`'s seven games reach only in Mario Kart World. Needs one RADV rebuild (new stamps `EDEN_MEMORY_PIECES_SHA256`, `EDEN_MEMORY_TRIM_SHA256`). Take it together with #75's script fix and #90's trim quarantine: without the quarantine the trim faulted the GPU (`GPU_FAULT_PAGE_FAULT_ASYNC`)
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10. The Mario Kart World memory stack: #68 is on `v2`, this is the next layer.
- What it takes: One conflict in `headless/performance.cpp` (keep `v2`'s 32 MiB gate). It brings `tools/patch-radv-pieces.py` and `tools/patch-radv-trim.py`, a RADV rebuild, and the tools half of #75.

### EglantinePS5 #95 (closed, not merged): memory given back in one step

- What it is: A real race `v2` has, per [prs/eglantineps5-port.md](prs/eglantineps5-port.md): `DecommitMemoryRange` (`src/memory_pages.cpp:840`), the `CommitMemoryRange` rollback (`:807`) and the window's `UnmapChunks` call `munmap` and then `sceKernelReserveVirtualRange(MAP_FIXED)`; between the two another mapping can land in the range and be replaced. `ReserveOver` reserves over the mapping in one call. Tested by its authors on firmware 13.60 in their own tree, only compiled against EglantinePS5's.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10. It is a correctness fix rather than a speed gain: the race would show as rare memory corruption under pool pressure.
- What it takes: It conflicts on `v2` alone (`src/memory_pages.cpp`) and is clean after #69 and #90: take it after them, or hand-port the two places `v2` has.

## Launcher (batch S5)

Cut by the owner on 2026-11-10. None of it changes how a game runs.

### EglantinePS5 #94: Start unencrypted games without keys or firmware

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `port`.
- Files outside the core: `headless/eglantineps5/eden_services.cpp`/`.h`, `host/fake_services.*`, `host/preview_main.cpp`, `pe/ui/launcher.cpp`, `services.hpp`, `settings.cpp`, `README.md`
- Applies to `v2`: clean, also alone on `v2` without #77 or #78
- Triage note: Launcher only. Extracted game folders (`.dir`), which is what the owner's games are, start without `prod.keys` and firmware; only `.nsp` and `.xci` are refused while they are missing (`Services::launch_blocked`). It names `.nup` by extension only, so it does not need #78. Adds launcher strings (see section 3 on the catalogs)
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10.
- What it takes: Applies clean, also alone. The owner's games are extracted folders, which this starts without `prod.keys` and firmware. It adds launcher strings.

### EglantinePS5 #50: Bananza Ingot Isle: coalesce feedback copies

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `adapt`.
- Files outside the core: `tools/bananza-bench-console.py` (new), `docs/BANANZA-INGOT-ISLE.md`
- Applies to `v2`: clean
- Triage note: The gain is the core patch: Ingot Isle's neon gate 9.3 to 15.0 FPS, Mario Kart World unchanged. The conflicts are on the core side (`v2`'s `alias_modification_tick`). The bench tool is new and comes as written: it drives Bananza to the gate from a pinned save and reads the frame lines, which is the A/B tool `v2` lacks for Bananza
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10. The core half is on `v2` (0018).
- What it takes: Only `tools/bananza-bench-console.py` and the doc are left: the A/B tool `v2` lacks for DK Bananza.

### EglantinePS5 #53: Bananza Ingot Isle: skip empty draws, draw-path measurements

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `port`.
- Files outside the core: `tools/bananza-bench-console.py`, `docs/BANANZA-INGOT-ISLE.md`
- Applies to `v2`: clean
- Triage note: Gate 15.0 to 15.35 FPS (about 3% of the draws there are empty). The bench now ends a run when a crash report appears. The doc lists experiments that did not pay (worth reading before repeating them)
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10. The core half is on `v2` (0019, `ce49770`).
- What it takes: The bench update and the doc's list of experiments that did not pay.

### EglantinePS5 #89: Console tools: ps5debug-NG memory maps and klogsrv kernel log

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `port`.
- Files outside the core: `tools/ps5debug.py`, `tools/klog.py`, `docs/CONSOLE-DEBUG-TOOLS.md` (new), `CONSOLE_COORDINATION.md`
- Applies to `v2`: clean
- Triage note: Tools only, nothing in the build. `klog.py` follows klogsrv on 3232, which `ps5 up` already starts. `ps5debug.py` needs the ps5debug-NG payload on the console (port 744), which is not part of the owner's setup; upstream notes it took the console off the network twice when reading a process that was exiting. Leave `CONSOLE_COORDINATION.md` out (it names another console)
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10.
- What it takes: Tools only. `klog.py` works with the owner's setup as it is; `ps5debug.py` needs a payload the console does not run.

### `install-app-files.py` pushes `ui/lang/`

- What it is: `/Volumes/Hangar/ps5/logs/install-app-files.py` installs only `eboot.bin`, `lapy.elf` and the manifest. A build that changes launcher strings or assets needs the rest of the package folder on the console.
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10.
- What it takes: Extend the script to upload and verify `ui/` (and `sce_sys/` when the icon changes), with the same backup rule.

### New icon, palette and display name

- What it is: The owner wants PlayStation colours and a new icon instead of EglantinePS5's pink flower. The palette is about 25 colours in `headless/eglantineps5/pe/ui/theme.hpp` (accent `kLime = 0xe23b60`) plus literals in `home.cpp` and `browse.cpp`; the icon is `sce_sys/icon0.png` (512 px) from `icon-source.png` (1024 px), with `pic0.dds` and `pic1.dds`; the name is `titleName` in `sce_sys/param.json`.
- Why it is out: Batch S5 (launcher), cut by the owner on 2026-11-10. Waiting for the owner's icon PNG and the decision on the name ("Horizon").
- What it takes: No upstream work to port: it is this fork's own change. `tools/check-launcher-theme.py` of #77 would have to follow the new palette.

## Measure before keeping (batch S6)

Cut by the owner on 2026-11-10. Each needs a console A/B to be kept or dropped.

### li#949: video_core: drain-free semaphore acquires, uniform keeps and descriptor replay on delta draws

- Source: [delta/gpu-core.md](delta/gpu-core.md), +1412 -50, 28 files, 1 commits, verdict `depends`.
- Triage note: Three things: semaphore acquires checked without draining the split, small uniform buffers kept between fast draw delta draws, and image descriptors replayed on them (Bananza gate 14.3 to 15.9 fps with all three, +0.7 from the acquires alone); the last two need li#858 (port, ps5 patch 0063), then measure on DK with their VERIFY switches; the acquire part overlaps v2's own ledger (puller.cpp QueuedRelease and AcquireChecksRawWordFirst, steps 36 and 38) and ps5 patch 0026, and its first commit is dj#639 (depends); its lock-free waiter latch and 1 ms drain valve (an MKW ghost replay hang) are worth lifting; 21 conflicts; anchors: buffer_cache.h, buffer_cache_base.h, pipeline_helper.h, vk_rasterizer.cpp, texture_cache.h; cmake: maxwell_3d.cpp, gpu_thread.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: 0063 is on `v2` now (`9107849`), so its uniform keeps and descriptor replay can be ported. Upstream it is a draft with 21 conflicts and one broken run. Its acquire part overlaps what 0026 (`6aeecb2`) now does on `v2`.

### EglantinePS5 #49: 64-bit fastmem window (`EDEN_FASTMEM64=1`)

- What it is: A 2^38-byte fastmem arena for 64-bit games (`src/host_memory.cpp` `Window64`, `headless/dynarmic/fastmem-callback.cmake`), off unless the switch is set. Triaged as `skip` in [prs/eglantineps5.md](prs/eglantineps5.md): the maintainer measured it slower on Mario Kart World at 1x (16.9 against 23.0 FPS median, 3 pairs), because most 16 KiB pieces are refused and page reuse turns off with an arena.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10. No measured gain anywhere; the only measurement is a loss.
- What it takes: It conflicts with ps5-pr#20 in `src/fastmem.h` and `src/guest_memory_service.inc`. Not worth porting unless upstream shows a game where it wins.

### EglantinePS5 #60: Graphics G1-G5 (#56-#59 inside): pacing, GPU memory, GPU thread, textures, core patches

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `measure`.
- Files outside the core: `headless/guest_clock.h`, `image_pages.h`, `texture_partial.h`, `astc_policy.h` (new), `headless/CMakeLists.txt`, `dev_vulkan.h`, `macro_native.cmake`, `main.cpp`, `performance.cpp`/`.h`, `vulkan_gc_downloads.inc`, `vulkan.cmake`, `tools/prepare-vulkan-port.py` (+728 lines), `tools/check-image-pages.py` (new), `check-startup-performance.py`, `check-worker-affinity.py`, `build-headless-host.sh`
- Applies to `v2`: conflict: `tools/check-worker-affinity.py` (keep both: #60's timing-based topology cases and `v2`'s #19 `x2apic_zero` case; `headless/performance.cpp` merges cleanly, CPUID first and the timing probe only when CPUID does not give five cores). 2 `prepare-vulkan-port.py` anchors to redo (Method)
- Triage note: Bananza gate 16.07 against 15.50 FPS (+3.7%); Mario Kart World 33.6 against 29.0 in one run, 29.4 against 25.2 median. After the merge 3 of 6 Bananza starts crashed until #75. The port side is four separable commits: `0b7eaef` pacing and placement (0.85x guest clock stretch, compose delay 1.5 ms, back thread core), `a74df7f` GPU memory (GC, 16 MiB allocator blocks, staging, pipeline cache), `0e5b103` GPU thread (deferred guest writes, write-ahead, image page map; thin LTO with `-march=znver2` for `video_core`), `0c0d063` textures (BC3 and GPU ASTC by default, 256 MiB transcode cap). Their kill switches are dev-settings knobs that only exist in `make dev` builds, so a release A/B needs two builds. The core patches marked `port` (0025, 0031, 0033) do not need this port side, except 0031's anchor change in `headless/macro_native.cmake`
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Four separable port-side commits (pacing, GPU memory, GPU thread, textures). One conflict in `tools/check-worker-affinity.py`. Take #85 with the pacing commit or not at all; #75's startup race fixes are already on `v2`.

### EglantinePS5 #85: Guest clock: a one-refresh title keeps an honest clock

- Source: [prs/eglantineps5-port.md](prs/eglantineps5-port.md), verdict `measure`.
- Files outside the core: `headless/guest_clock.h`
- Applies to `v2`: clean only after #60 (the file is new there)
- Triage note: A fix to #60's pacing: a title that shows a frame every refresh no longer has its clock stretched, which parked it on exactly 51.000 fps (60 x 0.85) with audio underruns. Comes with #60's `0b7eaef` or not at all
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Applies only after #60's `0b7eaef`.

### dj:1f52ac7e2e: nx2: drop late movie frames so the picture keeps up with the sound

- Source: [delta/services.md](delta/services.md), +98 -3, 1 files, 1 commits, verdict `port`.
- Triage note: The video decoder session drops decoded frames a later frame has overtaken on a movie clock, so Ocarina of Time's cutscenes keep up with their audio below 60 fps (117 s of movie took 148 s at 48 fps); applies clean to nx2_gsv.h and EDEN_NX2_VDEC_PACE=0 turns it off; take it with li#809's Nx2MovieClock, because this first clock ran ahead in Kirby Air Riders (25 of 588 frames shown), and see dj#737 (gpu-core) for Mario Kart World's movie pacing on the same code; ps5 patches 0037 and 0038 edit the same decoder class.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Applies clean to `nx2_gsv.h`; `EDEN_NX2_VDEC_PACE=0` turns it off. Take it with li#809's movie clock.

### li#809: nx2: keep refused decoder packets and stop the movie clock running ahead (#659 follow-up)

- Source: [delta/services.md](delta/services.md), +255 -64, 7 files, 1 commits, verdict `depends`.
- Triage note: Needs dj:1f52ac7e2e (port) and li#659's CoreTiming::GetPauseEndTime; then port its movie clock half (nx2_movie_clock.h): the clock starts at the first acquire and no longer follows frames acquired ahead, which fixes Kirby Air Riders' start-screen clips showing 25 of 588 frames; its refused-packet half edits li#659's queue refusal on Nx2VideoInputQueue, which v2 does not have; conflicts: nx2_gsv.h, core/CMakeLists.txt.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Needs dj:1f52ac7e2e. Take the movie clock half; the refused-packet half edits code `v2` does not have.

### li#412: video_core: drop redundant per-draw and per-frame bookkeeping on the GPU back thread

- Source: [delta/common.md](delta/common.md), +133 -4, 8 files, 1 commits, verdict `port`.
- Triage note: Four GPU-thread savings measured on Ocarina of Time (about 2.5% + 3% of the back thread): RangeSet::Subtract returns when nothing intersects, the query cache erases a page's queries only when one matched, UsageTracker::Reset skips untouched trackers, and PrepareDraw passes its pipeline to UpdateDynamicStates; none is in v2; conflicts: usage_tracker.h (v2 lacks the full_begin fields), query_cache_base.h (patch 0004's VisitQueryRegion holds that loop), vk_rasterizer.h; anchors: vk_rasterizer.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Conflicts in `usage_tracker.h` and `query_cache_base.h`. One of its four savings (untouched trackers) is already on `v2` through dj:1b7995d89c (`71c8415`).

### li#481: common: back RangeSet with a sorted array

- Source: [delta/common.md](delta/common.md), +167 -57, 2 files, 1 commits, verdict `depends`.
- Triage note: Needs li#412 (port; it removes the Subtract early-out li#412 adds), then port: RangeSet becomes a sorted array instead of a boost interval_set with pooled nodes behind a mutex, 5.0% of the GPU back thread in Ocarina of Time (30,000 lookups and 18,000 subtractions a frame), and it brings the moved-from clear and the missing `return *this` of operator= that v2's range_sets.inc lacks; conflict: range_sets.inc.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Needs li#412. One conflict in `range_sets.inc`.

### li#446: gpu_thread: poll suspended channels every 50 us instead of every 1 ms (Ocarina ~11% faster in Kokiri village)

- Source: [delta/gpu-core.md](delta/gpu-core.md), +16 -4, 1 files, 1 commits, verdict `measure`.
- Triage note: The GPU thread polls a channel suspended on a semaphore every 50 us instead of every 1 ms after the first 500 us (Ocarina suspends 330 times a second: its present waited 17 ms a frame, 1 ms after; about 11% faster in Kokiri village); applies clean, but the gain needs 50 us sleeps to be short on the PS5 kernel (its prctl timer slack is Linux only) and the port rewrites this file; try it on OoT and on Pokopia's waited acquires; li#621 follows it; cmake: gpu_thread.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Applies clean. The gain depends on 50 microsecond sleeps being short on the PS5 kernel, which nobody measured.

### li#979: video_core: restore the Switch 2 semaphore mirror, shadowing words with direct releases

- Source: [delta/gpu-core.md](delta/gpu-core.md), +269 -15, 7 files, 1 commits, verdict `measure`.
- Triage note: Restores Nx2SemaphoreMirror, which copies an NVN2 queue's releases from the second page of its two-page region to the first page that GPU acquires read, and adds a shadow table for words that also get direct releases (Pikmin 4's sky lighting); the pin lost the mirror in 60ef6d2750, so v2 registers the regions (nx2_gsv.h:513) and nothing mirrors them; v2's games run without it and no saved session has a 2 s semaphore wait line, so run it on the seven games and watch for hangs or new waits; hand port, its puller hunk uses GpuSplit::Run and Nx2FastDraw, absent from v2; conflicts: puller.cpp, kepler_compute.cpp, query_cache.h; cmake: puller.cpp.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: No saved session shows the 2 s semaphore wait it removes; take it only if one appears.

### li#816: video_core: mark native compute storage bindings only as far as the shader's stores reach

- Source: [delta/shaders.md](delta/shaders.md), +1105 -28, 14 files, 1 commits, verdict `measure`.
- Triage note: Marks native compute storage bindings GPU-written only as far as the shader's stores reach (StorageBufferDescriptor::write_extent) instead of the whole binding of up to 8 MiB: OoT 9-27 GB/s of fence copy-backs; the same over-marking is the likely source of DK Bananza's 100-230 MB/s fence downloads and Pokopia's copy-backs, where v2 only has the opt-in `EDEN_NX2_DOWNLOAD_DECAY`. Conflicts in the storage buffer pass, `buffer_cache.h` (anchors), `buffer_cache_base.h` (cmake rewrite), `tests/CMakeLists.txt` and it adds a setting: A/B the download bytes with `EDEN_NX2_GPU_PROFILE`.
- Why it is out: Batch S6 (measure), cut by the owner on 2026-11-10.
- What it takes: Large (14 files) with conflicts in the storage buffer pass. It is the likely fix for DK Bananza's 100-230 MB/s of fence downloads, so it is the first S6 item to measure.

## Found on the console, no upstream fix

From [console-findings-20261110.md](console-findings-20261110.md). Nobody upstream has a change for these.

### Pokopia uploads two textures again every frame

- What it is: Two textures are uploaded from guest memory each frame, about 4 MiB a frame.
- Why it is out: No upstream fix found in the three trees.
- What it takes: Find which write invalidates them (a texture cache trace of the two addresses), then decide.

### DK Bananza is limited by the GPU back thread

- What it is: 2,300-3,000 submissions a second and about 720 synchronous readbacks a second. 24.9 fps on the reference route (`dk-run3-noprof-1249`).
- Why it is out: Batch S3 on `v2` attacks it (0026, 0027, 0028, 0018, 0061, 0062, 0030, 0063); not yet run on the console.
- What it takes: After S3 is measured: li#816, then li#949.

### `tools/check-semaphore-acquire-clock.py` does not compile

- What it is: The script lifts the Ampere branch of `Puller::ProcessSemaphoreTriggerMethod` into a test program. `v2`'s branch calls the ledger and release tracking helpers (`QueuedRelease`, `AcquireChecksRawWordFirst`, `MetByPendingRelease`, `MetAfterPendingRelease`), which the script does not stub, so `clang++-18` rejects it.
- Why it is out: No build script runs it; EglantinePS5's copy is the same file.
- What it takes: Stub the four helpers and the `u64` and `HW_GPU` names in the script, or drop it.

### `make test` never completed

- What it is: The last run was stopped by hand at step 49 of 1,532. Some upstream tests need `tests/core/system_test_context.h`, which `v2` does not have (li#1196's ADPCM test was left out for it).
- Why it is out: Out of scope of the sync.
- What it takes: Bring the test context header from Lichii, then let one run finish.
