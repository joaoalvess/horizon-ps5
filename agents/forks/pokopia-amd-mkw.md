# Pokopia black sky on PS5 RADV, and the fork fixes for Pokopia, MKW and AMD

Written 2026-11-06 20:17. Research only: nothing was ported, built or run. Paths are relative to
`core-v2` (`/Volumes/Hangar/ps5/core-v2`, branch `horizon-v2`, head `cc70030a81`) unless they
start with `headless/` (the port, `.claude/worktrees/v2/headless`). In the files checked (`nx2.h`,
`buffer_cache_base.h`, `buffer_cache.h`), the port's `Eglantine/src` is identical to core-v2.
Session evidence comes from the 2026-11-06 19:11 build, `run3/eden_log.txt` and `run3/stderr.log`.

## What the session shows

- Settings line: `fast GPU on ... reactive flushing off`. `headless/settings_store.h:634-639` turns
  both on for every title with `(title_id >> 56) == 0x04`, so the title runs at gpu_accuracy Low.
  The desktop default is High (`common/settings.h:513-517`). The PC and Steam Deck runs that drew
  the sky correctly used High.
- `MaybeStart: GPU thread split: engine methods run on a second thread, macros on the first` at
  6.105 s. The split only runs below High.
- `Unsafe NX2 preemptive CPU read bypass active: skipping ordered completion` at 60.945 s.
  `EDEN_NX2_UNSAFE_PREEMPTIVE_READS=1` is a PS5-only default (`headless/main.cpp:538-544`, set
  with overwrite 0). This line falls inside the 15 ms when the world's 3D textures are created
  (3D TICs at 60.93-60.97 s), which is when the sky starts to be computed.
- No `EDEN_ENV` lines, so no env.txt was present. No rejected native program and no
  `Draw skipped` line.
- No `Sparse layered image` line, and only one `MKW_SPARSE_GATE` line (sparse=false).
- No `Native MME DMA read` line, although they are logged for the first 400 reads.

## Ranked hypotheses for the black sky

### 1. GPU-computed sky constants never reach guest memory: unsafe preemptive reads plus Low-accuracy fences

The core already records that the sky depends on the CPU reading data the GPU wrote. The comment
at `nx2.h:28-38` reads: "0400A3A01EE4C000 is Pokemon Pokopia: with the copy-back off its sky is
black or flickers". `buffer_cache_base.h:587-591` adds that the game "builds its sky, fog and
exposure constants from values the GPU computes". Pokopia is listed in `Nx2NeedsBufferDownloads`,
so `AsyncBufferDownloads()` is true: every GPU-written range is copied back to guest memory when
its fence is popped.

On the PS5, four things combine:

1. **The syncpoint is signalled before the copy-back lands.** `fence_manager.h:86`:
   `delay_fence = IsGPUFenceBehaviorDefault() ? IsGPULevelHigh() : ...`. At Low with Default
   fences, `func()` (the syncpoint's `IncrementHost`) runs at `SignalFence` time. The download is
   written to guest memory only later, in `TryReleasePendingFences -> PopAsyncFlushes`
   (`fence_manager.h:209-224`). At High the callback waits for the pop.
2. **The CPU's read is not ordered.** There is no fastmem arena on the PS5
   (`headless/main.cpp:534-537`), so every guest read of a GPU-tracked page goes through
   `HandleRasterizerDownload -> GPU::OnCPURead` (`core/memory.cpp:914-921`).
   - With `AsyncBufferDownloads()` true, `FlushesOnCpuRead()` is false
     (`buffer_cache_base.h:255`). `RasterizerVulkan::GetFlushArea` then leaves the buffer cache
     out and returns `preemtive = true` (`vk_rasterizer.cpp:1213-1245`).
   - `gpu.cpp:262-277` sees that flag with `EDEN_NX2_UNSAFE_PREEMPTIVE_READS=1` and returns
     without `gpu_thread.ReadMemory`, so the guest reads whatever the last popped download left.
3. **A pending copy-back can be starved.** When the GPU writes a range again,
   `buffer_cache.h:1807-1812` subtracts that range from every pending download ("its bytes are
   older than the write"). The guest runs ahead (immediate syncpoints, plus the split's queue).
   If the next frame's compute rewrites the range before the previous fence pops, that download
   never writes it. The guest then keeps reading the initial value (zero) for good. Exposure
   or sky colour at zero gives a pure-black sky, while geometry lit by other terms still draws.
   This step is inferred from the code and has not been measured.
4. **The read-trap fallback is off.** Read traps require a fastmem arena
   (`core/memory.cpp:603-611`), so they are off on the PS5. Reactive flushing is off as well.

Tests, cheapest first:

- env.txt: `EDEN_NX2_UNSAFE_PREEMPTIVE_READS=0`. Reads go through `gpu_thread.ReadMemory`, which
  flushes the region on the GPU thread (`gpu_thread.cpp:40-75`). If the sky returns, this is the
  cause. Expect a frame-rate cost: the comment says MKW's title screen ran 13 FPS without the
  bypass and 20 FPS with it.
- Library > Pokopia > Performance: `fast_gpu` off, which gives High accuracy. That makes the
  fence callback run after the pop, stops the split and enables MustFlushRegion. Then turn
  `reactive_flushing` on as a separate run.
- If High fixes the sky and the bypass alone does not, bisect at Low with env.txt
  `EDEN_NX2_SPLIT=0`. Fence behaviour has no runtime switch on the PS5 (dev-settings has none
  either), so separating it needs a Mario Tennis Fever style override. Mario Tennis Fever has
  Low plus `GpuFenceBehavior::Balanced` in `core/game_settings.cpp:139-143`, and Balanced makes
  `delay_fence` true.
- Optional log: `EDEN_NX2_READBACK_DIAGNOSTICS=1` (read in `video_core/readback_diagnostics.h:38`),
  plus `EDEN_NX2_READBACK_DEPENDENCY_DIAGNOSTICS=1`, logs the readback events.

A fix, once confirmed, belongs in the core and not in a fork commit. Either the bypass must not
treat a range with pending downloads as preemptive, or Pokopia gets Balanced fences in
`LoadOverrides`. The second is the existing per-title mechanism, but it is a workaround.

### 2. Other ordering differences of Low accuracy and the split (PS5 default only)

The same settings change other paths:

- `maxwell_3d.cpp:233/255`: the macro parameter fast path, and RefreshParameters only at High.
- `vk_rasterizer.cpp:1203`: texture GPU-modified state is not reported below High.
- `query_cache.h:283/336` and `fence_manager.h:86`: query and fence sync.
- `headless/vulkan_query_cache.cpp:1526-1618`: `hcr_mode=1` evaluates conditional rendering on
  the CPU when `!is_gpu_high`.

A draw predicated off by a lost query report leaves no log line, as with Mario Tennis Fever's
characters.

The two fork fixes for split ordering are not in v2:

- `0bd707c725` makes front-thread macro DMA reads wait for semaphore releases queued on the back
  (DK caves, Splatoon Raiders). The session logged no native MME DMA read, so it is unlikely for
  Pokopia's sky.
- `b12e25f166` fixes stale instanced-draw arguments read by the split's front thread.

Tests:

- The `fast_gpu` off run from hypothesis 1 covers all of these paths.
- At Low: env.txt `EDEN_NX2_SPLIT=0`, then `EDEN_NX2_SPLIT=1` (split on, but macros run on the
  back).
- `EDEN_NX2_QUERY_TRACE=1` for conditional rendering.

### 3. PS5-only NIR compute backend on the sky and volume compute

The sky's inputs are made by compute every frame:

- `0x2326cf0400` (1x1x1, 1539 instructions), `0x2326cee900` and `0x231d3ed600`.
- `0x231d3ed900` (8x8x8; it probably writes the 32x32x32 volume).

`headless/nir_backend.cpp` is on by default and exists only on the PS5, so a translation difference
there would not show on PC or the Steam Deck.

Test: env.txt `EDEN_NX2_NIR=0` with `EDEN_NX2_COMPILE_TIMES=1` (the latter logs fallbacks). If
the sky comes back, try `EDEN_NX2_NIR_EXTENDED_COMPUTE=0/1` and dump the five programs with
`EDEN_NX2_SHADER_DUMP=<dir>` and `EDEN_NX2_SHADER_DUMP_ALL=1`. For a barrier-only cause, try
`RADV_DEBUG=syncshaders`.

### 4. Stale shader inputs: the native capture and the pipeline cache's environment facts

- Native capture reads guest code with `ReadBlockUnsafe` (`shader_environment.cpp:432,485`). With
  reactive flushing off, code the GPU wrote is not flushed first. `a1f781fa24` fixes this; it is
  not in v2 and applies cleanly.
- The disk cache key hashes the lowered code (CityHash ^ `0x4f554e4345000002`). The environment
  facts are stored and trusted on reuse but are not hashed: texture types, pixel formats,
  `cbuf_values`, storage pointer types and cbuf replacements. Facts recorded with a broken
  earlier build survive a fixed lowering. A community report says deleting the Vulkan pipeline
  cache fixed a black sky.

Test: move `/data/eglantineps5/user/cache/shader/0400a3a01ee4c000/` aside
(`vulkan.fork-v28.bin`, `vulkan_pipelines.fork-v28.bin`, `vulkan_used.bin`), then boot twice.
The first boot rebuilds the cache, the second reuses it. A sky that is right on the first boot
and black on the second points to the stored facts. Also try `EDEN_NX2_SHADER_REUSE=0`.

### 5. Custom border colours removed (PS5 only)

`headless/dev_vulkan.h` sets `disable_custom_border = true`, because custom colours made some
games fault the GPU at page 0 (cause open). When the extension is removed,
`ConvertBorderColor` (`vk_texture_cache.cpp:88-104`) snaps a non-standard border to opaque white,
opaque black or transparent black. A sky or haze lookup that clamps to a custom border would
change.

Test: a dev build (`EDEN_DEV_PROFILE`) with dev-settings `custom_border=on pipeline_trace=on`.
The trace reports non-standard border colours.

### Lower priority

| Suspect | Why lower | Test |
|---|---|---|
| Sparse unbacked images | The session's only gate line says sparse=false; no `Sparse layered image` | env.txt `EDEN_NX2_SPARSE_UNBACKED=0` |
| Wave64 emulation (`emit_spirv_warp.cpp`) | Reviewed and correct; graphics pipelines never chain a required subgroup size, so fragment shaders run wave64 | env.txt `RADV_PERFTEST=pswave32` |
| ASTC to BC3 transcode | Affects texels, not a hard black | `EDEN_NX2_KEEP_ASTC=1` |
| ACO optimisation | No evidence | `ACO_DEBUG=noopt` (or `novn`, `nosched`) |
| D24S8 shown as D32_SFLOAT_S8 (RADV has no D24) | The Steam Deck, also RADV, draws the sky | no toggle |
| 32x32x32 volume layout (block_depth 4) | Suspected by the issue; no PS5-specific code path found | `EDEN_NX2_TEXTURE_DUMP` / `EDEN_NX2_DUMP` for that TIC |
| Undefined values | Already zero (`EDEN_NX2_UNDEF_ZERO`) | none |

### Variables the port already reads that apply here

- Applied through env.txt: `EDEN_NX2_UNSAFE_PREEMPTIVE_READS`, `EDEN_NX2_SPLIT`,
  `EDEN_NX2_BUFFER_DOWNLOADS`, `EDEN_NX2_CPU_READ_RANGES`, `EDEN_NX2_READ_TRAPS`,
  `EDEN_PS5_UNWRITTEN_READS`, `EDEN_NX2_PUBLISHED_SEMAPHORE_READS` (High only),
  `EDEN_NX2_READBACK_DIAGNOSTICS`, `EDEN_NX2_NIR`, `EDEN_NX2_NIR_EXTENDED_COMPUTE`,
  `EDEN_NX2_COMPILE_TIMES`, `EDEN_NX2_PIPELINE_STATS`, `EDEN_NX2_SHADER_REUSE`,
  `EDEN_NX2_SHADER_DUMP` (with `_ALL` and `_MAPPED`), `EDEN_NX2_REJECT`, `EDEN_NX2_KEEP_ASTC`,
  `EDEN_NX2_SPARSE_UNBACKED`, `EDEN_NX2_UNDEF_ZERO`, `EDEN_NX2_DUMP`, `EDEN_NX2_TEXTURE_DUMP`,
  `EDEN_NX2_IMAGE_TRACE`, `EDEN_NX2_RT_TRACE`, `EDEN_NX2_QUERY_TRACE`,
  `EDEN_NATIVE_DRAW_DIAGNOSTICS`, `EDEN_LOG_FILTER`, `EDEN_NX2_VDEC_EOS_RELEASE_NOW`,
  `EDEN_NX2_FRAMES_AHEAD`, `RADV_DEBUG`, `RADV_PERFTEST`, `ACO_DEBUG`.
- Not env.txt: Library > Performance (`fast_gpu`, `reactive_flushing`), and dev-settings.txt in
  `EDEN_DEV_PROFILE` builds (`gpu_accuracy=low`, `custom_border=on`, `pipeline_trace=on`,
  `hcr=eden/exact/cpu`, `sparse=off`, `robustness2=on`, `radv_debug=`, `env=`).

## Fork commits

Apply checks ran against core-v2 with `git apply --check`, forward and reverse. "Clean" means the
forward apply passed; "already in" means the reverse apply passed.

| Game | Fork commit(s) | What it fixes | In v2? (evidence) | Matters on PS5 RADV? | Applies to core-v2? | Recommendation |
|---|---|---|---|---|---|---|
| Pokopia | 1bd952c53b | Guarded ULEA without .HI or carry out: the PC black sky (#387) | Yes (`lower_to_maxwell.h:163-191`; general UISETP at 1573) | Already fixed | Conflicts (equivalent present) | None |
| Pokopia | ce07064323 | F2I abs/neg; constant LEA without carry out; STS 0x988 (night) | Yes (`lower_to_maxwell.h:2980` has the fork's check; `0x988` handled; F2I fixed) | Night shaders | Conflicts (equivalent present) | None |
| Pokopia | 1eef3d9806 | SHF.L.U32.HI imm, I2F.U8 byte 1-3, USHF any UR, UPRMT imm, DEPBAR.LE (night compute) | Yes (`case 0x419` 2731, `0x306` byte select 2747-2759, `0x91a` 2033, UPRMT 0x896 -> 0x816 at 164-193) | Night compute | Conflicts (equivalent present) | None |
| Pokopia | 8ef7f1ea41 | Drops Pokopia's full buffer copy-back (32 -> 16.7 ms on PC) | No (`nx2.h:33-38` still lists the title) | Yes, for performance and for the sky's readback (hypothesis 1); the PS5 has no read traps (`memory.cpp:603-611`) | Clean | Do not port yet. Measure with env.txt `EDEN_NX2_BUFFER_DOWNLOADS=0` (same effect) after hypothesis 1 is settled |
| Pokopia | a1f781fa24 (dj#337) | Flushes GPU-written code before capturing native shaders | No (`ReadBlockUnsafe` at `shader_environment.cpp:432,485`) | Yes: reactive flushing is off, so nothing else flushes | Clean | Port (1) |
| Pokopia, AMD | 77cbc3e7c3, c193314a73 | Counts nested returns before moving or combining demotes (AMD black scene) | 77cbc3e7c3 no; c193314a73 partial | No: `needs_demote_reorder` is off on RADV | 77cbc3e7c3 clean; c193314a73 conflicts | Skip |
| AMD | 68caa6c75e | AMD proprietary shuffle fallback, CAS retry reload, depth-compare float typing (RADV `spirv_to_nir` crash, OoT), unoptimized builds | Typing hunk yes (`texture_pass.cpp:1341`); the rest AMD proprietary | Typing hunk already in; the rest no | Conflicts (15 errors) | None |
| AMD | 1b8d62d8b7, 86be145af9 | AMDVLK graphics shuffle loop and required subgroup size; not for task and mesh shaders | No | No: gated to AMD_PROPRIETARY, AMD_OPEN_SOURCE and SAMSUNG, not RADV; bumps the cache to 29 | Conflicts | Skip |
| AMD | 8f3d47232c | Chains the compute subgroup size only when `requiredSubgroupSizeStages` has COMPUTE | No | No-op: RADV GFX10.3 lists COMPUTE | Clean | Optional; correct on other drivers |
| MKW | 9d86952d4f | Forces subgroup 32 only for compute with warp operations (Intel SIMD32 spills) | No | Maybe for performance: RADV compiles compute as wave64 unless forced | Conflicts | Measure (`EDEN_NX2_PIPELINE_STATS=1`, compile times) |
| AMD | 3d0ab32c3c | Backs a sparse layered image whole when the driver's block is not one page (black terrain on RX 9070 XT) | No (`InitSparseLayers` at `vk_texture_cache.cpp:2693` trusts the granularity) | Not for Pokopia (no `Sparse layered image` line); MKW unknown | Conflicts | Measure on MKW: grep `Sparse layered image` block sizes; port only if RADV reports a block that is not one page |
| AMD | 6bde8c20d1, 9b815ed9bf, a855e8373f, b96b0e1cff, 46d1554186, 93e9e7e344 | AMD sparse backing policies; zero block-size divide | No | No: the port forces `EDEN_NX2_SPARSE_UNBACKED=1` (`headless/main.cpp:521`); b96b0e1cff is AMD proprietary only | b96b0e1cff clean; the rest conflict | Skip (93e9e7e344 only if a zero-granularity divide shows up) |
| AMD (diagnostic) | 6280be89fc | `EDEN_NX2_DRIVER_AMD` takes AMD proprietary paths on any card | No | No: changes no hardware behaviour | Clean | Skip |
| MKW | 91ec32e668 | Black screen when a Grand Prix starts: a decoder release after end of stream completes at once | Yes, opt-in (`nx2_gsv.h:2283-2295`, `EDEN_NX2_VDEC_EOS_RELEASE_NOW`) | Yes, if the PS5 hangs there | Conflicts (already there) | Test env.txt `EDEN_NX2_VDEC_EOS_RELEASE_NOW=1` at a GP start; make it the default if it helps |
| MKW | 800f7feaf6 | Text: RET's reserved field is not a write to R0 | Yes (`lower_to_maxwell.h:123` `!no_destination`) | Already fixed | Conflicts (equivalent present) | None |
| MKW | 3fdec64d93 | Eyelids and dark materials: I2F.F32.U64 0x312, I2F immediates 0x845/0x906 | Yes (`0x312`, `0x845`, `0x906` present) | Already fixed | Conflicts (equivalent present) | None |
| MKW | 0cd801d8b9 | Dynamic frame rate: PaceToVblank matched the title through GetCurrentProcess | v2's `PaceToVblank` (`nx2_gsv.h:1523`) has no per-title check; it differs | Unknown | Conflicts | Check whether MKW stays at 30 FPS on the PS5 before adapting |
| MKW | 690f80e15f | Present stall waiting for the current frame | Different mechanism (`nx2_gsv.h:1440-1444`, `EDEN_NX2_FRAMES_AHEAD`); dj#337 carries it | Possibly | Conflicts | Use env.txt `EDEN_NX2_FRAMES_AHEAD=1` if presents stall |
| MKW | aa31176874 | Docked photo-mode upscale stand-in (rejected tensor passes) | No (no `PHOTO_UPSCALE`) | Docked photo mode only (the PS5 runs docked) | Conflicts | Defer: a workaround heuristic, 178 lines |
| DK, split | 0bd707c725 | Black 3D scene in DK caves and Splatoon Raiders: front macro DMA reads wait for releases queued on the back | No (`macro.cpp:1390` reads with no wait; `puller.cpp:83` `NoteRelease` serves acquires only) | Yes for DK: the PS5 runs `EDEN_NX2_SPLIT=2` (macros on the front); not Pokopia (no MME DMA reads logged) | Conflicts (4 errors), needs adapting | Port (2), adapted |
| Fast Fusion, split | b12e25f166 | Split front read stale instanced-draw arguments (hang) | No | Yes whenever the split is on | Conflicts; needs dj#322 | Port (3), with dj#322 |
| Split | 427b9abfdc | A split instance loop restarts at its own instance index | No (no counter in `draw_manager.cpp:226-252`) | Possible wrong instance IDs at Low | Conflicts | Measure; adapt if instanced draws look wrong |
| OoT | 636c5fb385 | Folds IMul32 by 0 and 1 so the sky ambient binds its buffer | Yes (`FoldIMul32` at `constant_propagation_pass.cpp:393`, half recombine at 342) | Already fixed | Conflicts (equivalent present) | None |
| Several | 354f72fa04, 07c017f1d3, c8bb9a3be0, fc9342cb26, 0804c28f0c, f259b38d65 | Vote folding, FSEL neg/abs, VOTEU UR16-27, TXQ forms, native timestamps | Not checked one by one; 8b4384ddab (F2I U64 saturate) is already in (reverse apply clean) | Shader-form fixes are not driver-specific | Conflict | Check per game when a rejection names the form |

## Port order

1. **Not a fork commit.** Settle hypothesis 1 on the console first (env.txt
   `EDEN_NX2_UNSAFE_PREEMPTIVE_READS=0`, then `fast_gpu` off). If confirmed, fix it in the core:
   either the unsafe bypass excludes ranges with pending downloads, or Pokopia gets Balanced
   fences.
2. `a1f781fa24`: applies cleanly; flushes GPU-written code before native capture.
3. `0bd707c725`, the macro and split part, adapted: DK caves on a split that runs macros on the
   front.
4. `b12e25f166` with dj#322: stale instanced-draw arguments on the split.
5. MKW: test `EDEN_NX2_VDEC_EOS_RELEASE_NOW=1` (91ec32e668 is already present, opt-in). Measure
   `3d0ab32c3c` and `9d86952d4f` before porting either.
6. After the sky is fixed: measure `8ef7f1ea41` with `EDEN_NX2_BUFFER_DOWNLOADS=0` for Pokopia's
   frame time.
