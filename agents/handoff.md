# Handoff, 2026-11-06 08:50

For the next agent. Everything here was measured on 2026-11-06; [PLAN.md](../PLAN.md) holds the stages, its [Next](../PLAN.md#next) section the ordered steps.

## Where things are

| What | Where |
|---|---|
| `v2` checkout | `/Volumes/Hangar/ps5/horizon-ps5/.claude/worktrees/v2`, branch `v2`, HEAD `a868abb` (the Qt, Android and CLI removal, kept last). Herdr worktree, workspace `w85`; tab `w85:t2` "build v2" (pane `w85:p2`) is idle |
| `v2`'s core as commits | `/Volumes/Hangar/ps5/core-v2`, branch `horizon-v2` at `cc70030a81`; its tree equals `v2`'s `HEAD~1:Eglantine` |
| Uncommitted in `v2` | `PLAN.md`, `README.md`, `agents/findings.md`, `agents/port-log.md`, `agents/console-test.md`, `agents/prs/`, this file |
| `v2` ZIP | `/Volumes/Hangar/ps5/dist/v2/EglantinePS5-v1.000.000.zip` (sha256 `9c73f86e46080102d142aeb93f7e8a9033ab5da785ee8a60ce78434db6128f06`), symbols ELF and `SHA256SUMS` beside it; built 02:31 from `a868abb`, every release check passed but `check-jit-allocator.py` (skipped under Rosetta) |
| `main` ZIP | `/Volumes/Hangar/ps5/dist/main/` |
| Upstream analysis clone | `/Volumes/Hangar/ps5/upstream/Eglantine`, remotes `djihads80` and `lichii`, fetched 08:4x today |
| EglantinePS5 clone | `/Volumes/Hangar/ps5/upstream/EglantinePS5` (not fetched today) |
| Rebase probe (scratch) | `/Volumes/Hangar/ps5/upstream/rebase-probe`: clone of `core-v2` with `lichii-nx2` and `dj-nx2` branches; `patches.txt` lists the 17 patch commits |
| Per-commit test harness (scratch) | `/Volumes/Hangar/ps5/logs/test-bisect/` (`one.sh`, `one2.sh`, `annotate.py`, `capture.py`) |

Build: `tools/docker/run.sh make JOBS=6 RADV_BUILD_JOBS=6` in the worktree ([docs/BUILDING.md](../docs/BUILDING.md#on-a-mac)). The owner runs processes in Herdr tabs of the worktree's workspace, not in an agent's own shell. Commit with the repository's identity, no AI attribution lines, and never push without the owner's word.

## Done since the last summary

- `check-nx2-core.py` failed on `v2`. Three core commits fix it, made on `horizon-v2` and transplanted before the Qt removal:
  - `f95684ab1d`: li#58's test called `LowerToMaxwellWith` in Lichii's argument order.
  - `9d6f094576`: TLD.SCR bit 91 (uniform-handle flag, required by `0xf66`'s case) was ignored on `0xb66`'s path, so such a fetch used the wrong texture. Now refused.
  - `cc70030a81`: the pin's Ampere tests (only in the pin's line, never run by Djihads80 or Lichii) refused 18 forms that ported units lower on purpose; updated unit by unit (message lists each).
  - Found by running the six failing test files against each of the 46 lowering commits (harness above). `nx2-core-check` now passes: 2,240,135 assertions, 204 test cases.
- `make` passed (log `/Volumes/Hangar/ps5/logs/make-v2-20261106-0228.log`); ZIP copied to `dist/v2`.
- `make test` was stopped by hand at step 49 of 1,532 (`/Volumes/Hangar/ps5/logs/test-v2-20261106-0232.log`); its configure passes now.

## Upstream findings (this morning)

### Lichii (zortos293/Lichii) and Djihads80 (Djihads80/Eglantine)

- The pin `3140648a66` is Lichii's own "Eglantine nx2 build 34". Lichii `nx2` at build 88 (`55532f2d81`, 2026-11-06 08:54 UTC) is 919 commits ahead of the pin and 0 behind.
- Lichii merges Djihads80 periodically; last merged Djihads80 commit `e736dfde9e` (2026-11-05). `djihads80/nx2` has only 9 non-merge commits Lichii lacks, all triaged: skip, or dj#639 (`df92fc6c95`, dropped as `depends`), dj#634 (`8ef3563080`, skip), dj#571 (`8820bb79aa`, skip). Lichii has 136 non-merge commits Djihads80 lacks; one is a revert (`a3e94bf4e9`, reverts Lichii #72).
- Of the 113 Djihads80 units `v2` took (picked, adapted, partial), 105 are ancestors of `lichii/nx2`: Lichii is close to a superset of what `v2` ported from Djihads80.
- `djihads80/nx2` has not moved since `0ca2032bff` (2026-11-05 21:58 +0100), the triage's tip. Djihads80 now sends unverified pull requests to `nx2-experimental`; active work is on branches `oot-optimization-fixes`, `fix/oot-blackscreen`, `scratch/oot-shader-lab`, `exp/nx2-welcome-tour-1121`.
- The triage covered `lichii/nx2` up to `5f6066b26b` (li#184, perf9, verdict `measure`). New on `lichii/nx2` since: #193 (tracker size type, macOS build), #194 (Frizti performance and FSR3 ports, `depends` on li#184 in [prs/lichii.md](prs/lichii.md)), #240 (Ocarina Dynamic FPS mod, on by default for OoT).
- Community view (guide `riyadhHDA/wichforks`, verified 2026-11-06): Djihads80 is the default pick; Lichii is the pick for Zelda OoT remake, Star Fox and Mario Tennis Fever. "Best" claims for Lichii come from it aggregating Djihads80, its own game fixes and other forks (Frizti, Dark), and from fast, well-written releases.

### Rebasing the core on Lichii: probe

`git merge-tree` of each PS5 patch alone onto `lichii-nx2` (in the probe clone): 16 of 17 conflict. Conflicted files per patch: 0001: 1, 0002: 2, 0003: 1, 0004: 54, 0005: 2, 0006: 13, 0007: 9, 0008: 3, 0009: 1, 0010: 3, 0011: 2, 0012: 2, 0013: 5, 0014: 1, 0015: 3, 0016: 0, 0017: 1. Some conflicts may be changes Lichii already has (patch 0003 is in dj:34e1f691ae, [findings.md](findings.md)). The port's anchors (`tools/prepare-vulkan-port.py`, the `string(REPLACE)` blocks in `headless/CMakeLists.txt`) would also need checking against Lichii's code. Decision open, the owner's: keep `v2` (pin plus cherry-picks) or move the core to Lichii plus the 17 patches. Suggested: console-test `v2` first, decide after.

### EglantinePS5 (heydemoura/EglantinePS5)

`main` moved past `44d0d9d`: PR #52 (`d79d75c`), with `8aa5625` "Build only the PS5 app: drop the desktop emulator from make test" (the 1,532-step desktop build `v2`'s `make test` still does) and `183f120` "Shader cache check: write entries with the core's surface_bound field" (touches `headless/shader_cache_check.cpp`, which `v2` changed in `8f8e052`). Not in `v2` yet.

### Open pull requests new since the [prs.tsv](prs/prs.tsv) snapshot (not triaged)

Listed by `gh pr list` at 08:4x. A first look by title marks with `*` the ones likely worth a verdict for the PS5.

Djihads80/Eglantine:

| PR | Base | Title |
|---|---|---|
| #663 * | fix/oot-blackscreen | macro: enlarge Turing macro RAM to 0x8000 words so entries past 0x1000 execute |
| #673 * | nx2-experimental | shader: give a transform feedback varying that starts inside a vector its own byte offset |
| #675 | nx2-experimental | externals: FFmpeg with AV1 on MinGW Windows |
| #676 * | nx2-experimental | kepler_compute: stand in for Mario Kart World's docked photo mode ups… |
| #678 * | nx2-experimental | shader/spirv: force float Sampled Type for depth-compare images |
| #680 | nx2-experimental | Optiscaler/Streamline integration fixes |

zortos293/Lichii (all on `nx2`):

| PR | Title |
|---|---|
| #213 | input: HD Rumble and DualSense haptics |
| #214 * | tools: rebuild Vulkan pipeline caches from playthrough hints |
| #215 * | video_core: wait for pending query results before macro DMA reads (Ocarina white flashes) |
| #216 * | core: keep Ocarina of Time at real speed below 60 fps |
| #219 | ci: verify packaged Windows AV1 decoding (draft) |
| #221 | file_sys: ignore IPSwitch lines that are not patch records |
| #223 | AMD Mobile GPU successful in-game launch (+13,301) |
| #224 * | video_core: opt-in EDEN_NX2_ASYNC_DISCARD to skip native draws with unbuilt pipelines |
| #228 | dist: describe EDEN_NX2_FRAMES_AHEAD as the code reads it |
| #230 | vulkan: float16 for host shaders so LSFG loads |
| #231 * | video_core: resolve waiting semaphore acquires without draining the GPU split's back (+3,094) |
| #232 | common: time-critical threads on performance cores, Windows |
| #235 * | video_core: queue a safe read's flush on the split's back thread |
| #241 * | shader: retry native lowering with an aligned high uniform base before rejecting |
| #243 * | video_core: macro DMA reads on the split front without draining the back |
| #245 * | video_core: read bindless handle words from the host-aligned storage base |
| #246 | tests: compare unsigned results with unsigned literals |
| #250 * | video_core: skip GPU-write invalidations of caches with nothing on those pages |
| #251 * | nx2:vdec: keep Ocarina's movies in time with their sound below 60 fps |
| #253 | video_core: DLSS in place of Ocarina's anti-aliasing |

### Djihads80 `measure` units not in `v2`, worth a console run

From [delta/](delta/): dj:6bc1ef08c2 (back GPU-only regions as written; MKW's ~3 GB of render targets in the 12 GiB pool), dj:0f14696044 (wait for the presented frame when the GPU is behind), dj:660e5f0039 (fence batching on the split), dj#334 (MKW performance bag), dj#233 (chunked staging uploads), dj:1f0e3b88c4 (DeviceBuffers pool), dj:70b39fe1d2 (drop Eden's pipeline cache copy on RADV, ~245 MB host memory on MKW, needs RADV added), dj#506 and dj#363 (Bananza), dj:2f94fb9680 (destroy pipelines unused for 9000 frames), dj:50fb1556cb (best-fit image placement). Deferred in [port-log.md](port-log.md): dj#322, dj#313 (multiplayer), li#112, li-pr#197.

## What is left, in order

1. Apply EglantinePS5 `8aa5625` and `183f120` to `v2` the way #19 and #46–#48 were (`git am -k -3`, then the `Pull-request: heydemoura/EglantinePS5#52 (<sha>)` trailer), after `git -C /Volumes/Hangar/ps5/upstream/EglantinePS5 fetch origin`. Keep the Qt removal last. Rebuild, then `tools/docker/run.sh make test JOBS=6`; stage 3 is done when both pass.
2. Commit the docs listed above (`docs(agents): …`).
3. An independent review of `v2`: history, authors and `-x` lines, `HEAD~1:Eglantine` against `horizon-v2`'s tree, the docs against git.
4. The owner: push `v2` and run its GitHub Actions build, or not.
5. Stage 4, the console A/B ([console-test.md](console-test.md)).
6. Triage the new pull requests above and Lichii #194 and #240 by [prs/README.md](prs/README.md), one row each in `prs/djihads80-new.md` and `prs/lichii.md`.
7. The owner: keep cherry-picking or move the core to Lichii (probe above).
8. Clean-up after stage 4: branch `v2-before-197-drop`, `/Volumes/Hangar/ps5/home` (old arm64 cache, 2.8 GB), `/Volumes/Hangar/ps5/logs/test-bisect/`, `/Volumes/Hangar/ps5/upstream/rebase-probe/`.
