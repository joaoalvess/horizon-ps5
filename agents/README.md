# agents

Triage of the upstream work the port's core does not have yet, to decide what `v2` takes and to measure it against `main`.

The core is the `Eglantine` submodule, pinned at `3140648a668aaebf87bae2dea17512fdca125ba0` (zortos293/Eglantine, 2026-11-01). Since then two lines moved on:

| Source | Branch | Ahead of the pin |
|---|---|---|
| eglantine | `Djihads80/Eglantine` `nx2` | 793 commits (549 non-merge) |
| lichii | `zortos293/Lichii` `nx2` | 916 commits; 774 shared with eglantine, 160 only in lichii (133 non-merge) |

Together: 666 non-merge commits, grouped into 503 units.

## Layout

| Path | What it is | Writer |
|---|---|---|
| `delta.py` | Builds the inventory from the analysis clone | whoever regenerates it |
| `delta/units.tsv` | One row per unit: source, date, author, title, size, primary subsystem, every subsystem it touches, `base` and `tip` | `delta.py` only |
| `delta/<subsystem>.md` | The units of one subsystem, with a Verdict and a Why column | the agent assigned to that file |
| `port.md` | How EglantinePS5 wraps the core and what the PS5 changes; read it before judging a unit | the main thread |

Single writer: each `delta/<subsystem>.md` has one agent that edits it, and nobody else does. An agent that finds something for another subsystem writes it in its own file's Why column ("see gpu-vulkan: dj#123") and reports it; it does not edit the other file.

## Units

`delta.py` walks each source's first-parent line from the pin, then lichii's minus eglantine's. A unit is:

| Id | Unit |
|---|---|
| `dj#N`, `li#N` | A pull request: its merge commit when every commit it brings is new and there are at most 40 of them, or a squash commit ending in `(#N)` |
| `dj:<sha10>`, `li:<sha10>` | A commit made directly on the branch |

Other merges (syncs between the two lines, merges of large branches) are not units: the walk descends into them and lists what they carry. The primary subsystem is the one with the most changed lines; lines in `src/tests/` count a quarter toward the subsystem they test.

## Reading a unit

The analysis clone is `/Volumes/Hangar/ps5/upstream/Eglantine` (remotes `djihads80` and `lichii`). The Diff column holds `base..tip`:

```console
$ cd /Volumes/Hangar/ps5/upstream/Eglantine
$ git log --oneline <base>..<tip>
$ git diff --stat <base> <tip>
$ git diff <base> <tip> -- src/video_core/renderer_vulkan/
```

The port's side is this repository: the submodule at the pin plus `tools/eglantine-patches/` is the core the PS5 build compiles, `tools/prepare-vulkan-port.py` rewrites 32 of its files at build time, and `headless/` is the PS5 frontend. `port.md` lists them.

## Verdicts

| Verdict | Meaning |
|---|---|
| `port` | Changes code the PS5 build compiles, and makes it faster, use less memory, or fixes something a game does on the console. Goes to `v2` |
| `measure` | A plausible gain that the PS5 may not see (written for another driver, CPU backend or memory layout). Goes to `v2` only after an A/B run shows it |
| `skip` | Not built on the PS5, only cosmetic for it, or worse for it. The Why says which |
| `in port` | The port already has it: one of the patches, `prepare-vulkan-port.py`, or code in `headless/`. The Why names which |
| `depends` | Needs another unit first. The Why names it, then gives the verdict the unit would have |

Every Why is one sentence with its evidence: the file and function, the driver or backend the change is for, the measurement. A unit that touches a file `prepare-vulkan-port.py` adapts or a file a patch changes says so (`anchors: vk_scheduler.cpp`, `patch 0011`): porting it means changing that anchor or patch too.

## Triage criteria

Before judging anything, read `port.md`. In short:

- Built on the PS5? The app links Eden's `core` with its own frontend (`headless/`). The Qt, Android and CLI frontends, CI, packaging for other systems and Windows or macOS code are not built.
- CPU: dynarmic's x86-64 backend runs on the PS5's Zen 2. Changes only to the ARM64 backend do not apply. Fastmem is off for 64-bit games, so every guest load and store goes through the page table.
- GPU: the Vulkan renderer runs on RADV (Mesa) for the PS5's RDNA2 GPU; OpenGL (ps5-opengl) is the fallback. A workaround keyed on another vendor's driver (NVIDIA, Qualcomm, Mali, Intel, MoltenVK) does not apply unless it also covers RADV.
- Memory: the title has one 12 GiB direct-memory pool shared by guest RAM, the page table and the GPU. A change that lowers memory use or allocation churn is worth more here than on a desktop.
- Games: the owner's games first (Mario Kart World, Donkey Kong Bananza, Pokémon Pokopia), then anything that fixes a crash or a hang in general.

## Regenerating

```console
$ python3 -I agents/delta.py --fetch
```

It fetches both remotes, rewrites `delta/units.tsv`, and appends units that are not listed yet to the subsystem files. Verdicts already written stay.
