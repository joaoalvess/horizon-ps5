# Console findings from the 2026-11-06/07 sessions

Writer: the log-analysis agent (2026-11-10). Scope: what the console session logs of build `v2` show to be broken or slow, per game, and which upstream change addresses it. Research only: no code changed, nothing built, the console not contacted.

Sources: `/Volumes/Hangar/ps5/logs/boot/` (every session folder), the crash reports in it, [PLAN.md](../PLAN.md) "Console games", [console-test.md](console-test.md), [forks/](forks/), [prs/eglantineps5-main.md](prs/eglantineps5-main.md). Upstream cross-references: `/Volumes/Hangar/ps5/scratch/triage-20261110/core` (`v2-current` 2e730c41e1, `lichii-nx2` 093e264c8a, `dj-nx2` 78743f30c2), `/Volumes/Hangar/ps5/upstream/EglantinePS5` (`origin/main` 095df4e, patches 0001-0071), `/Volumes/Hangar/ps5/mihawk-mesa-review` (the RADV fork `v2` builds).

Conventions: **Shown** is what a log line states (path and line given). **Inferred** is my reading and is marked. "In v2" means present in `v2-current`.

## Coverage

| Game | Sessions on the Mac | Builds |
|---|---|---|
| Pokopia (`0400A3A01EE4C000`) | 9 folders (`pokopia-*`), plus the first 1,157 s of `mkw-freeze-1322` | logs build, perf, perf3 |
| DK Bananza (`040074A01BF12000`) | `dk-full-0141`, `dk-rumble1-0231`, `dk-listdrain-live`, `dk-gpuprofile-1145`, `dk-run1-1230`, `dk-run2-1237`, `dk-run3-noprof-1249`, `games-perf-20261107/022615-…`, `dk-check-0138`, `vibration-0137` | perf, rumble, perf3 |
| Mario Kart World (`0400C3F00006E000`) | `mkw-freeze-1322` (run 1), `mkw-run2-live` (run 2) | perf3 (`0d408203a1+101-changed-96b4ee85b0`, compiled Nov 7 15:22:43) |
| Zelda OoT, Kirby Air Riders, Mario Tennis Fever, The Duskbloods | **none** | - |

`games-perf-20261107/` holds four sessions and all are Pokopia or DK: `013557` is `pokopia-run-perf-A-defaults`, `022015` is `dk-full-0141`, `024116` is `dk-rumble1-0231`, `022615` is a DK session found nowhere else. `collect-sessions-20261107.log` saved those four and then only failed polls. No klog exists for any MKW or DK session (`/Volumes/Hangar/ps5-lab/runs` holds one Pokopia klog of 2026-11-06).

## Mario Kart World: the GPU stops completing work 36-40 s after the game loads

### Symptom

Both sessions fail entering the first 3D scene (the title's), on the same build:

- Run 1 (second game in the process, download decay `pages`): 60 fps in the 2D loading, then one 7.9 s frame and one frame per 5.3 s for 66 s; the crash handler ran at 1322.98 s (it copied the logs; its report is 0 bytes) and the console froze.
- Run 2 (first game in the process, no decay): 60 fps until 144 s, 39.6 fps in the next window, then `VK_ERROR_DEVICE_LOST` at 160.35 s; the app stopped itself and wrote a full report.

### The last 15 s of run 2

File `mkw-run2-live/crash-20261107-140333-eden_log.txt` (858 lines), `…-stderr.log`, `…-heap.log`, `crash-20261107-140333.txt`.

| Time (s) | Line | Shown |
|---|---|---|
| 110.000-110.024 | 89-123 | Seven "Patch topology not used with tessellation, using patch list" while the disk pipeline cache loads (176 pipelines, line 129). Seven is the two plus five that run 1 built |
| 111.88-111.90 | 269-280 | Six 285 MiB GPU-only regions backed; six `216 map … of memory 30 … into a GPU-only region: keeping its backing` |
| 119.51-119.53 | 367-378 | Ten `216 map [..., +0x10000] of memory 1354 … keeping its backing` (16 "keeping its backing" lines in all, the same 16 in run 1) |
| 128.30 | 388-391 | Four sparse regions (1.42, 5.67, 1.42, 1.42 GiB) "backed as the GPU writes it" |
| 128.51 | 394 | GPU-only region `[0x8430230000, 0x859afaffff]` (5.67 GiB, no sparse flag) backed whole: GPU-only memory in use goes 2,667 to **8,472 MiB** |
| 128.68 | 396 | `Service <Error> nx2_gsv.h:945 AllocateGpuOnlyBacking: nx2:gpu 214 could not allocate 0x5ad80000 bytes`: the next region, `[0x859afb0000, 0x85f5faffff]` (1,453 MiB), gets no backing and no mapping |
| 139.047-139.059 | 504-521 | Sixteen `Unmapped Device ReadBlock` (reads of 69,632 bytes). The message is capped at 16 per process (`device_memory_manager.inc:528`), so the real count is unknown |
| 143.32 | 559 | One more 341 MiB GPU-only region backed (8,813 MiB in use) |
| 144.09 | 587 | `EDEN_FRAME fps=60.3` |
| 145.03-145.28 | 591-778 | Scene load. Eight `MKW diagnostic: task mesh draw groups=1` at 145.2355-145.2379 (lines 708-728). The diagnostic is capped at eight per process (`vk_rasterizer.cpp:714`), so later task/mesh draws are not logged |
| 146.27-146.54 | 795-821 | Five `InitSparseTiles` images (three 8192x8192, two 16384x16384), each followed by `View of unbacked image` |
| 148.797 | 827-843 | Seventeen native shaders lowered in 0.6 ms, no pipeline build logged (inferred: pipelines found in the disk cache). Sizes: 672, 1120, then 1072 + 3168 + 528, then four pairs 1088 + 3168, then 2096 + 432 and 2336 + 352 bytes |
| 149.098 | 844 | `EDEN_FRAME fps=39.6 p99_ms=200.5 worst_ms=896.3 total=501`. The last frame line of the session |
| 150.271 | 846-848 | Three shaders lowered (944, 2080, 384 bytes) |
| ~150.349 | - | Inferred from the watchdog (below): the last time the GPU completed a submission |
| 150.368 | 850-854 | Five more "Patch topology not used with tessellation" (five new pipelines) |
| 150.370-150.5075 | 855-857 | A 256,144-byte compute shader lowered; "Compute pipeline built on the GPU thread in 118.5 ms"; a graphics pipeline in 15.4 ms |
| 160.349289 | 858 | `Render.Vulkan <Critical> vk_master_semaphore.cpp:126:Wait: Waiting for GPU tick 6959 failed: VK_ERROR_DEVICE_LOST` |

`stderr.log`: the last `radv/ps5 submissions` window has `count=5378 kwords=6357 agc_ms=146.5` (line 126), so submissions were not blocking before the stop. Line 140: `radv/ps5: the GPU has not reached submission 9173 (at 9172) in 10 s`. Line 141: `[EglantinePS5] terminate: VK_ERROR_DEVICE_LOST`. `EDEN_MEMORY`: largest free block 7,920 MiB, `short=0`.

### The last 15 s of run 1

File `mkw-freeze-1322/crash-20261107-132916-eden_log.txt` (MKW header at 1220.4 s, line 4033).

| Time (s) | Line | Shown |
|---|---|---|
| 1238.73 / 1238.895 | 4335 / 4337 | The same 5.67 GiB region backed whole (8,472 MiB) and the same `could not allocate 0x5ad80000 bytes` |
| 1247.838-1247.855 | 4428-4446 | The same sixteen `Unmapped Device ReadBlock` |
| 1252.9 | 4499 | 60.1 fps |
| 1254.094-1254.108 | 4621-4671 | Shaders of 432, 6208 and 1616 bytes lowered, "Patch topology not used with tessellation", then immediately `MKW diagnostic: task mesh draw groups=1` (lines 4628-4629); a second such pipeline and seven more mesh draw lines |
| 1255.35-1255.68 | 4744-4772 | The same five sparse tiled images with unbacked views |
| 1256.687-1256.694 | 4775-4803 | The same shaders as run 2's 148.797, here with five "Patch topology…" warnings interleaved: 672, 1120, 1072, 3168, 528, warning, then four times (1088, 3168, warning) |
| 1264.535 | 4804 | `EDEN_FRAME fps=8.7 worst_ms=7858.9`: the long frame began at about 1256.68 |
| 1269.8-1322.7 | 4806-4838 | One frame per 5.25-5.32 s; `gpu_split.cpp:259 Drain: … back running (inside a command)` at 1285.5, 1301.4, 1306.6, 1311.9 |

`stderr.log` line 875: `window count=1833 kwords=2940 agc_ms=7885.1`. Then windows of 43-52 submissions and 39-46 kwords with `agc_ms=10496-10570`, the whole window (lines 907-923). No "has not reached submission" line: no single stall reached 10 s. Largest free block 4,472 MiB.

### What the crash address is

`eboot+0xb47e5` is `main::$_10::__invoke()` in `headless/main.cpp`, the frontend's terminate hook (the one that prints `terminate: VK_ERROR_DEVICE_LOST`). Its caller on the stack, `eboot+0x14ef87c`, is `Vulkan::MasterSemaphore::Wait` (`vk_master_semaphore.cpp:126`). Symbolized with `tools/symbolize-crash.py`. Caveat: no symbol ELF of the perf3 build exists; the perf (`dist/v2-perf-20261107`) and perf4 (`build/symbols`) ELFs agree on the first address and only perf4 resolves the second.

So this is not a CPU fault. The driver's stall watchdog (`radv_ps5_queue_wait_seq`, `radv_ps5_winsys.c:86-116`, 10 s without marker progress) returned device lost and the app stopped on purpose.

### What the logs establish

1. **The GPU was busy, not faulted.** Run 1 kept completing a frame every 5.3 s for a minute. Each window then held about 45 submissions of about 1,000 command words each, and `sceAgcDriverSubmitDcb` blocked for the whole window. A few small command streams that each take a quarter of a second or more are draws or dispatches that amplify inside the GPU (task/mesh expansion, tessellation, instancing, a dispatch grid), not long command lists. Inferred.
2. **The hung submission of run 2 predates the five pipelines built at 150.368.** The watchdog resets its timer on every completed submission and fired at 160.349, so nothing completed after about 150.349, and submission 9173 already existed then. The five warnings come 19 ms later, at pipeline build, which is before their first draw is recorded. PLAN's next step ("the shader dumps of the pipelines built at 150.37 s") therefore looks at pipelines that cannot be in the hung submission. The suspects are draws recorded before 150.349: those using the shaders lowered at 148.797 and 150.271, or older ones.
3. **Those warnings do not prove tessellation.** `vk_graphics_pipeline.cpp:828` raises it whenever stage slot 1 or 2 holds a module, and a Switch 2 mesh program sits in the tessellation slot (`shader_environment.cpp:662`). The two pipelines at 1254.09 are task/mesh for certain (mesh draw line 85 us later, three programs each). For the five at the stall the logs cannot say. Inferred: they are task/mesh too. The lowering pattern is one triple (1072, 3168, 528 bytes) and then four pairs (1088, 3168) that reuse the 528-byte program, which fits five task + mesh pairs sharing one small fragment program better than five-stage tessellation pipelines whose evaluation and geometry stages change together. No "Skipping tessellation pipeline" line exists in either run.
4. **One 1,453 MiB GPU-only region has no memory in both runs.** The 5.67 GiB region before it lacks the sparse flag, is backed whole from the guest kernel's system pool, and exhausts it. `SubCreateGpuOnlyRegion` (`nx2_gsv.h:665`) then maps nothing for the next region: GPU writes there are lost and reads return zero. This is the only Error-level line specific to MKW, it is 22 s before the stop in run 2 and 18 s in run 1, and upstream measured exactly this state ("82 regions, 8,830 MiB; one 1,453 MiB region got no backing and stayed unmapped", `docs/MKW-MEMORY-BUDGET.md`).
5. **Sixteen sparse maps are skipped on v2 only.** Batch 1's "GPU-only regions under sparse maps" (made for Bananza) keeps the GPU-only backing where MKW maps memory 30 over its six render-target regions and two 64 KiB pieces of memory 1354 at ten addresses. The baseline applied those maps.
6. **Ruled out by the logs:** pool exhaustion on the host (largest free block 4,390-7,920 MiB, `short=0`); rejected native programs (no `rejected` line); draws past 2^24 vertices or indices (no `Draw skipped`), index bindings of 16 MiB or more (no `MKW_INDEX_SPAN`); the macro event upstream sees at race load (no `instruction limit exceeded`, no `Native MME failed`); a second game in the process and the download decay (run 2 had neither).

### Ranked causes

| Rank | Cause | For | Against | Kill switch or test |
|---|---|---|---|---|
| 1 | Task/mesh draws that amplify: a task program emitting a mesh group count read from data that is missing or wrong | Timing in both runs (first mesh draws 2.6-5 s before, the five likely task/mesh pipelines 0-1.6 s before); the shape in point 1; RADV's task shaders are emulated; upstream found the same class in OoT (task draws reading culling records nobody wrote, patch 0071) | The baseline also issues task/mesh draws and renders the title at 23-26 fps, so the trigger must be data that differs on `v2` | `EDEN_NX2_NO_MESH=1` |
| 2 | Data the GPU-driven path reads is absent on `v2`: the skipped 216 maps (point 5), the unmapped 1,453 MiB region (point 4), or the read-path changes of steps 36-38 | Point 5 is `v2`-only and logged; point 4 is an Error in both runs | Point 4 also happens on upstream builds that render the title; the memory 1354 maps look like placeholder tiles (the same 64 KiB at five addresses) | `EDEN_NX2_SPARSE_MAP_GPU_ONLY=1`; `EDEN_NX2_GPU_HELD_PAGES=0`, `EDEN_NX2_READ_WAIT_TO_WRITER=0`, `EDEN_NX2_ACQ_LEDGER=0`; EglantinePS5 0022 |
| 3 | Native tessellation with a geometry stage, admitted since batch 2 (before it the pipeline was dropped) | `v2`-only; the RADV fork documents that a tessellated geometry shader amplifying past 256 vertices loses the device, and `v2` lacks upstream's `patch-radv-gs-compute.py` sinks | Point 3: the pipelines are more likely task/mesh; in run 2 the same shaders ran for 1.5 s before the stop | `EDEN_NX2_REJECT=<the tess program addresses>` once the dumps name them |
| 4 | A compute dispatch or an indirect or instanced draw with a garbage count | `v2` guards only direct draws' vertex and index counts; upstream's race-load event feeds a float (0x3F800000) to a draw macro as a count and ends in "a slideshow (0.1-0.7 fps) … or a device lost", the same two outcomes | Upstream's event comes at race load and logs macro failures, which `v2` does not | The draw ring below |
| 5 | Sampling the five unbacked sparse images | They appear 1-4 s before the stop | The port forces `EDEN_NX2_SPARSE_UNBACKED=1` on the baseline too; frames completed after the views were made; a fault would not leave the GPU completing frames | `EDEN_NX2_SPARSE_UNBACKED=0` (memory cost) |
| - | Host pool out of memory; impossible index spans; RADV trim page fault | - | Point 6; `v2` has no `patch-radv-trim.py` | - |

### Upstream changes per candidate

| Candidate | Change | In v2 |
|---|---|---|
| Tessellation patch-list handling | dj `3a7477111f` (orchid `8bbf3e71f8` and guard `bce763d65b`) | yes (batch 2). li `7a9dd37103` was dropped in stage 3 |
| Task/mesh through compute, without `VK_EXT_mesh_shader` | li#569 `b7ff181f20` | no. `v2` draws them with `VK_EXT_mesh_shader` (dj `02504aeb37`, patch 0004); the switch dj `6d9821d50b` is in |
| Task/mesh draws reading unwritten records | EglantinePS5 0071 (OoT only, `EDEN_OOT_MESH_DRAWS`) | no (verdict adapt) |
| GPU-only region with no backing | EglantinePS5 0022, `b63c3b2` (#67): regions of 16 MiB or more backed as the GPU writes them | no (verdict adapt: extend batch 1's `InGpuOnlyRegion` to lazily backed dense regions first) |
| Sparse image backing | li#880 `25a28bb4ec`; li `16967eb30e`, `a855e8373f` | no. `6bde8c20d1` is the pin; the port forces unbacked |
| Impossible index spans | EglantinePS5 0056 (#90) | half: `v2` has the 2^24 draw guard (li#142, `3e77e72dcc`); the 64-bit span and the 128 MiB cap in `UpdateIndexBuffer` are not in |
| Dropped GS-compute draws faulting the GPU | EglantinePS5 `92531b2` (`tools/patch-radv-gs-compute.py`) | no |
| RADV trim page fault | EglantinePS5 `9f3f7f5` | not applicable (no trim in `v2`) |
| Draw and dispatch diagnostics | dj `450331d35b` (`EDEN_NX2_DIAG`, `EDEN_NX2_DIAG_DIR`, `EDEN_NX2_ABLATE_FILE`) | no |

### Diagnostic recommended before any MKW boot

Offline first, no boot:

1. Pull `/data/eglantineps5/shaderdump` (run 2 set `EDEN_NX2_SHADER_DUMP`; the dumps are on the console, not on the Mac) and MKW's `vulkan.fork-v31.bin`. Read the stage slot of each program below: this settles task/mesh against tessellation, and shows where the task programs take their group count from. Run 2 addresses: `0x114d3fb100` (1072), `0x114d3fb600` (3168), `0x114d3fae00` (528), the pairs at `0x114d3fc400`/`0x114d3fc900`, `0x114d3fd700`/`0x114d3fdc00`, `0x114d3fea00`/`0x114d3fef00`, `0x114d3ffd00`/`0x114d400200`, and the three lowered 78 ms before the stop: `0x114d3f1f00` (944), `0x114d403e00` (2080), `0x114d403b00` (384).

Then a diagnostic build, logging only:

2. Per new pipeline: the stage kind in each slot (task, mesh, vertex, tessellation control and evaluation, geometry), topology, patch control points, and the program addresses.
3. A ring of the last draws and dispatches per submission (kind, count, instances, first, mesh groups, indirect count and its first records, dispatch grid, pipeline hash), written to `stderr` when a submit blocks for more than 1 s. The cap of eight on the mesh draw line goes.
4. A stall limit of 2-3 s for this build (`RADV_PS5_STALL_NS` is 10 s) and no restart after the stop. Run 2 ended cleanly because the 10 s limit was reached; run 1 never reached it, ran the GPU saturated for 66 s, and the console froze once the crash handler ran.
5. The kernel log recorded from before launch (port 3232). No klog exists for either run, and the kernel names GPU faults.

Boot order, one change per boot, stopping at the title: first `EDEN_NX2_NO_MESH=1` (cause 1), then `EDEN_NX2_SPARSE_MAP_GPU_ONLY=1` (cause 2), then 0022 once ported.

## DK Bananza: 13-29 fps in play, limited by the back GPU thread

### Symptom

Menu 60 fps, cutscenes 30 (the game's interval), play 14-29 fps with frames of 34-73 ms. Session means over the whole run: 34.6-46.5 fps; `dk-run3-noprof-1249` in play: 20-28 fps (`eden_log.txt` lines 1750-2006).

### Evidence

The profile PLAN records was re-read and holds (`dk-run2-1237/eden_log.txt` line 1625, back thread per second at 142.6 s): unzoned 161 ms, method decode 146 ms (1.36 M calls, 44 % of the 3D writes the same value), split wait 134 ms, update buffers 121 ms, bind stage buffers 87 ms (198,322 calls for 87,616 draws), configure 55 ms, draw 51 ms, fill image views 41 ms, flush work 37 ms, read handles 26 ms, pipeline lookup 26 ms, push descriptors 20 ms, fence 20 ms. Front (line 1612): split drain 522 ms (8,962 a second), push 229 ms, macro 176 ms.

Beyond PLAN:

- **2,300-3,000 queue submissions a second** in play (`dk-run3-noprof-1249/stderr.log` lines 261-268: `count=23013…29670` per 10 s window, `agc_ms` 940-1,015, `suspend_ms` 1,125-1,177). That is about 100 submissions a frame, against 8 a frame in Pokopia.
- **About 720 synchronous buffer readbacks a second**: `ReadbackDiag … buffer_readback calls=7169 … buffer_finish calls=7169 … device_finish calls=7169` per 10 s (`dk-run3-noprof-1249/eden_log.txt` line 2012), 29 samples taking 4.97 ms. Inferred from the sample: about 170 us each, about 120 ms a second on the thread that waits.
- **85 % of semaphore acquires wait for the back**: `64000 checked, 9426 met by the guest word, 0 met by a queued release, 54560 waited for the back` (line 1925). The ledger meets none.
- **Every GPU-written list waits for the back and none needed it**: `536000 checked, 536000 waited for the back (34 words each), 0 needed … 692 had a noted writer queued` (`dk-run2-1237/eden_log.txt`, 148.1 s).
- Colour feedback: `ReplaceColorFeedbackViews: Colour target 2880x1620 … sampled by its own draw: copying it` (4-12 lines a session, first at 25.3 s). The line is not a count; the copies are not zoned in the profile and would sit in the unzoned 161 ms.
- No rejected native program, no skipped draw, no pool pressure (largest free block 3,230-4,514 MiB) in any DK session.

### Candidate fixes

| Cost | Change | In v2 |
|---|---|---|
| Per-draw setup: update buffers, bind stage buffers, configure, image views, pipeline lookup, read handles (about 355 ms a second) | EglantinePS5 0063 (#92): fast draw delta path and texture memo, li#858 `c7e807d617`, li#921 `1c25fbdfb4`, li#910. Upstream: gate 15.8-16.0 to 16.9-17.1 fps, per-draw setup 5.7 to 2.7 us; Lichii 11.65 to 14.83 fps | no (verdict adapt; needs 0029 and 0026) |
| Bind stage buffers, update buffers, push descriptors, acquire waits | li#949 `a4f6287327`: drain-free acquires, uniform keeps, descriptor replay on delta draws (builds on the fast draw) | no |
| Image copies for colour feedback | EglantinePS5 0018 (#50): one copy per texture barrier and render-target set. Upstream gate 9.3 to 15.0 fps (copies 17,600 to 2,600 a second) | partly: `v2` has the alias tick (batch 2) and its own `ReplaceColorFeedbackViews`; the coalescing is not in (verdict adapt) |
| Front waiting on GPU-written lists and macro reads | li#1056 `1a5e995a38` (macros that read memory run on the back), li#752 `3099486e5b` | no. The back is the limit, so expect little until the per-draw cost falls |
| Upload scans | dj#562 (step 41) | in the perf4 build, not yet measured on the console |
| Method decode and same-value 3D writes (146 ms a second) | none found upstream | - |

Not found upstream: a fix for the 100 submissions a frame or the 720 readbacks a second. Both deserve a counter by cause (which flush reason, which reader) before any change.

## Pokopia: 54-58 fps on the island

### Symptom

`pokopia-run-perf-A-defaults`: 56-59 fps after 190 s with `p50_ms` 16.6-17.4 and `p99_ms` 19-29 (`eden_log.txt` lines 1152-1246). Presents: `frame 17095-18000 us … vblank pacing 25-72 us` (lines 1638-1707), so the frame has no slack.

### What remains in the frame (shown)

- **Two texture uploads from guest memory every frame, 4 MiB a frame**: `Texture cache last 600 frames: 1200 uploads from guest memory, 2400 MiB (largest 4 MiB, 0 by layer)` in every window from 241 s (lines 1310-1712), with `created 0 images, deleted 0` beside it. The same images are uploaded again each frame: 240 MB a second. The menu uploads one small image a frame (line 356). Inferred: unswizzling 4 MiB on the GPU thread is the largest single item above the 16.7 ms budget. Not noted in PLAN.
- **46 % of semaphore acquires wait for the back**: about 2,100 acquires a second, `147649 waited for the back` of 320,000 (line 1195).
- 475 submissions a second (`stderr.log`: `count=4751 kwords=63492 agc_ms=286.2 suspend_ms=662.8` per 10 s); `upload_staging calls=168449 bytes=2118451008` per 10 s (line 1461); 14 runtime pipelines in one 10 s window costing 598 ms on the GPU thread (line 1562), then 2-3 per window.

### Repeated warnings and errors

No Error-level line beyond the two every game has (`Failed to read title for ROM!`, `nacp_unique is null!`). Warnings are stubs only: `MKW_LAYER_STREAM` (11), `MKW_GC` (6), nx2 stub devices, `ResolveCallerProgramId` fallbacks. No rejected program, no skipped draw.

The 19-minute Pokopia run inside `mkw-freeze-1322` ended at 1154.65 s with `SetTerminateResult … result=0x1a80a (2010-0212)` and `Userspace PANIC!` (lines 3947-3950), followed by `EDEN_GAME_SESSION_END`. `EDEN_PAD_CLOSED` reports `intercepted=2895 circle=1902`. Inferred: the owner's exit shortcut, not a fault; the only such line in any session.

### Candidate fixes

| Cost | Change | In v2 |
|---|---|---|
| Per-frame re-upload of two images | None identified upstream. EglantinePS5 0061/0062 (li#817, li#818) skip uploads of new images only. First log which images (address, size, format, who marked them modified) | - |
| Acquires waiting for the back | li#949 `a4f6287327` | no |
| Runtime pipeline stalls | li#71 `f8f29b5769` (verdict measure) | no |

## Zelda OoT, Kirby Air Riders, Mario Tennis Fever, The Duskbloods

No session of these four exists on the Mac: no `EDEN_SESSION game` line names their titles in any log under `/Volumes/Hangar/ps5/logs` or `/Volumes/Hangar/ps5-lab/runs`. Nothing about boot, frame rate, rejected shaders or crashes can be taken from logs. [console-test.md](console-test.md)'s result rows for them are empty for the same reason.

What is known without a run is in [forks/oot-kirby.md](forks/oot-kirby.md), [forks/dk-tennis.md](forks/dk-tennis.md) and [prs/eglantineps5-main.md](prs/eglantineps5-main.md) rows 0054, 0068, 0069, 0071 (OoT). One boot of each with `EDEN_LOG_FILTER=*:Info Shader:Debug` gives the rejection counts those notes wait for.

## Across games

**Errors and criticals** (13 distinct session logs: 8 DK, 3 Pokopia, 2 MKW):

| Line | Sessions | First seen | Note |
|---|---|---|---|
| `Core <Error> Failed to read title for ROM!`, `nacp_unique is null!` | all | at load | Extracted dumps without a control NCA. Harmless |
| `unimplemented function '1050'` on `nx2:IGraphicsScaleController` | 13 | during boot | li#56 implements it; asserts are soft (K6, skip) |
| `unimplemented function '1'` on `IUserShimScopedObject` | 12 | during boot | - |
| `Unmapped Device ReadBlock` (16, the cap) | every DK and MKW session, no Pokopia session | DK 21-30 s, MKW 27-29 s after load | Reads of 49,152-69,632 bytes that return zero. Possibly orchid `a1c562051b`/`7826be2b17` (sparse native uniform data across remaps, [forks/dk-tennis.md](forks/dk-tennis.md) line 50, skip) |
| `unimplemented function` on `nx2:camera` (3, 21, 29), `nx2:chat:gsv` (103, 121), `nx2:chat:u` (3), `nx2:chat:u::0` (2) | both MKW runs | 4.5 s after load | - |
| `unimplemented function '903'` on `nx2:gsv` | `dk-rumble1-0231` only | 213.19 s | Once |
| `AllocateGpuOnlyBacking: could not allocate 0x5ad80000 bytes` | both MKW runs | 18 s after load | See MKW point 4 |
| `Userspace PANIC!` | the Pokopia part of `mkw-freeze-1322` | 1154.65 s | See Pokopia |

**Rejected native programs:** none in any session (`Native graphics shader rejected`, `Native compute shader rejected` and `Rejected program` never appear). **Skipped draws:** none.

**Memory pool:** no pressure anywhere. `EDEN_MEMORY` never reports `short` above 0; the smallest largest-free-block is 3,230 MiB (`dk-rumble1-0231`). The only memory failure is in the guest kernel's system pool (MKW point 4).

**Other:** `Geometry shader passthrough used with no support` once per DK session at 11.5 s (`emit_spirv.cpp:359`); the semaphore ledger meets 0 acquires in every game.

## The five changes to port first, from the logs alone

1. **EglantinePS5 0022** (`b63c3b2`, +16 -1), with `InGpuOnlyRegion` extended as its verdict says. It removes the one reproducible Error in MKW and frees 8.8 GiB of the guest kernel's pool. Not proven to be the hang.
2. **MKW guard and diagnostics**: the `UpdateIndexBuffer` half of EglantinePS5 0056 and dj `450331d35b`, with the diagnostic build above. MKW stays unbootable until one boot can name the draw.
3. **EglantinePS5 0063** (li#858, li#921, li#910): the per-draw cost that limits Bananza.
4. **li#949** `a4f6287327`: the acquire waits seen in Bananza (85 %) and Pokopia (46 %), and Bananza's buffer binds.
5. **EglantinePS5 0018**'s feedback-copy coalescing, after a counter confirms how many copies `v2` makes in Bananza play.

For Pokopia no upstream change targets the main finding (the per-frame re-upload); it needs one diagnostic line first.
