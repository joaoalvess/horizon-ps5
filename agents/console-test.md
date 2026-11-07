# Console test, `main` against `v2`

Stage 4 of [PLAN.md](../PLAN.md), by hand: each game on both builds, same settings, to see what the ported units changed before the scripted MKW bench (`tools/mkw-bench-console.py`) measures it. Writer: the main thread.

## The builds

| Build | ZIP | Core |
|---|---|---|
| `main` | `/Volumes/Hangar/ps5/dist/main/EglantinePS5-v1.000.000.zip` | the pin plus the 17 patches |
| `v2` | `/Volumes/Hangar/ps5/dist/v2/EglantinePS5-v1.000.000.zip` | the same, plus the units [port-log.md](port-log.md) marks `picked`, `adapted` or `partial` and EglantinePS5 #19, #46, #47 and #48 |

Both are release builds. Install one, test, then install the other over it (`make install PS5_HOST=<address>` from each checkout, or the ZIP's folder over FTP to `/data/homebrew/PPSA98515`).

## Shader caches

`main` and `v2` translate shaders differently. A title's pipeline caches are kept apart by version: `main` writes `vulkan.bin` and `vulkan_pipelines.bin` (Vulkan cache version 25), `v2` writes `vulkan.fork-v28.bin` and `vulkan_pipelines.fork-v28.bin` (li#150), and `v2` adopts an old-named file only when its header has version 28, so neither build reads the other's. `vulkan_used.bin` beside them (the pipelines recent sessions used, built first at boot) is `v2`'s alone. What both builds share needs no swapping: `vulkan_static_pipelines.bin` (in `/data/eglantineps5/user/cache/shader/`) is a Vulkan pipeline cache the driver keys by pipeline, and RADV's disk cache (`/data/eglantineps5/cache/radv/`) is keyed by the SPIR-V, so the same shader compiles to the same binary for both.

The first boot of each game on each build compiles its shaders. Measure from the second boot.

## What to check per game

Each row says what `v2` should change, the units that change it, and what to write down. Record for both builds: boots or not, seconds to the title screen, FPS on the HUD at the same spot, the crash and the seconds after launch if it dies, and any wrong picture (screenshot with `ps5 shot`).

| Game | What `v2` should change | Units | Watch |
|---|---|---|---|
| Mario Kart World | Survives past 170–220 s, where `main` runs out of the 12 GiB pool (GPU-only backings re-registered without release); no crash after 4.5 min from a uniform buffer past 2^36; boots from an NSP with the Switch 2 layout and clock | dj:c7c97429e1, dj#260, dj#349 | Seconds until `VK_ERROR_OUT_OF_DEVICE_MEMORY` or device lost, if any; FPS at the title (baseline 23–26) |
| Donkey Kong Bananza | No abort after the cutscene movies (post-stream fences completed early); NSP boots with the Switch 2 layout | dj#356, dj#349 | Play past the first cutscenes; terrain |
| Pokémon Pokopia | Photos are no longer black (compute lowering: DEPBAR, SHF.L.U32.HI, LEA, USHF); does not stop after its first frame | dj:201aacdae5, dj#337 | Take an in-game photo |
| Zelda: Ocarina of Time | No startup abort (`WaitIfEqual64`, arbitration type 3, was rejected) | dj#567 | Reaches the title |
| Kirby Air Riders | TLD4 with a uniform handle lowered instead of refused | the TLD4 commit of dj#270's lowering set | Missing or wrong textures against `main` |
| Mario Tennis Fever | Native tessellation stages admitted instead of the pipeline being dropped | li:7a9dd37103 | Courts and characters drawn |
| The Duskbloods | Switch 2 detection and the general lowering units | dj#349, li#119 | Boots, reaches gameplay |

A unit that [port-log.md](port-log.md) marks `deferred` or `dropped` does not reach `v2`: its row above then shows no change, and that is expected.

## Measure next

`measure` changes are not in `v2` unless a row says so. These are the ones worth a console run after the comparison above, most promising first. Each comes with its file row.

| Change | Where `v2` stands | How to measure |
|---|---|---|
| JIT direct table, page reuse and fast paths (ps5-pr#46–#48) | in `v2`, on by default as #48 sets them | Same build, variants `EDEN_JIT_TABLE=0`, `EDEN_JIT_PAGE_REUSE=0`, `EDEN_JIT_FAST_PATHS=0` (`env.txt`); the maintainer measured 29.8 against 22.6 FPS on MKW's title with the fast paths ([prs/eglantineps5.md](prs/eglantineps5.md)) |
| RADV's PS5 queue wait on the monotonic clock, and the 64 MiB shader-module budget (ps5-pr#11, two parts) | not in `v2` | Today the 1 ms queue wait spins; CPU time of the GPU threads and FPS, MKW |
| Fence batching on the split GPU thread (dj:660e5f0039, with li#59, dj:313463cd27 and li-pr#149) | not in `v2` | MKW title FPS and a long Kirby Air Riders session (the crash the group fixes is in `CommandChunk::Record`) |
| x86-64 JIT acquire loads as plain moves, no `mfence` for DMB LD/ST (li-pr#161, d51b04d80a alone) | not in `v2` | CPU-bound scenes, every game; any new hang or corruption is a memory-ordering bug |
| Skip upload scans of buffers the guest has not written (dj-pr#562) | not in `v2` | MKW FPS, and corruption on screen |
| MKW device memory from 2 MiB pieces, budget counted once (ps5-pr#20, draft) | not in `v2` | Seconds until MKW runs out of the pool, if `v2` still does |
| Descriptor tables sized to their limits, gathered buffers released (dj-pr#277, dj-pr#276, drafts) | not in `v2` | `MKW_MEMORY` and device memory over a long MKW run |
| Ocarina of Time's black gameplay scene (dj-pr#614, draft) | not in `v2` | Zelda: OoT gameplay picture |
| Switch 2 movies with four decoder threads (dj-pr#488) | not in `v2` | Bananza's cutscene movies: smoothness and audio sync |

## Results

| Game | `main` | `v2` | Notes |
|---|---|---|---|
| Mario Kart World | | | |
| Donkey Kong Bananza | | | |
| Pokémon Pokopia | | | |
| Zelda: Ocarina of Time | | | |
| Kirby Air Riders | | | |
| Mario Tennis Fever | | | |
| The Duskbloods | | | |
