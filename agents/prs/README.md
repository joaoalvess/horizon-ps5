# Open pull requests

The triage in [../delta](../delta) covers the branches of Djihads80/Eglantine and zortos293/Lichii (`zortos293/Eglantine` redirects there). Most open pull requests come from forks, so their commits were never in it. This folder judges them by the same rules ([../README.md](../README.md#verdicts), [../port.md](../port.md)), so that `v2` can take the ones that help on the PS5.

## Files

| File | Pull requests | Writer |
|---|---|---|
| [prs.tsv](prs.tsv) | every open one on 2026-11-06, from `gh pr list` | the main thread |
| [djihads80-new.md](djihads80-new.md) | Djihads80/Eglantine #530 and later | its triage agent |
| [djihads80-old.md](djihads80-old.md) | Djihads80/Eglantine before #530 | its triage agent |
| [lichii.md](lichii.md) | zortos293/Lichii | its triage agent |
| [eglantineps5.md](eglantineps5.md) | heydemoura/EglantinePS5, the PS5 port itself | its triage agent |

Each file has one row per pull request:

| PR | Updated | Title | Size | Needs | Verdict | Why |
|---|---|---|---|---|---|---|

- **Size**: `+added -removed, files` from `prs.tsv`.
- **Needs**: what must be in `v2` first: delta units (`dj#270`, `li:7a9dd37103`, as in `../delta/units.tsv`), other pull requests (`dj-pr#593`, `li-pr#204`, `ps5-pr#46`), or `-`.
- **Verdict**: as in the delta files, plus `in v2` when [../port-log.md](../port-log.md) already takes the same change. A pull request whose change is already a delta unit says `in delta: <unit>` in its Why and takes that unit's verdict.

## Where the code is

| What | Where |
|---|---|
| Djihads80 and Lichii pull request heads | `/Volumes/Hangar/ps5/upstream/Eglantine`, refs `refs/remotes/djihads80-pr/<n>` and `refs/remotes/lichii-pr/<n>`; their base branches are `djihads80/<base>` and `lichii/<base>` (`base` column of `prs.tsv`) |
| EglantinePS5 pull request heads | `/Volumes/Hangar/ps5/upstream/EglantinePS5`, refs `refs/remotes/pr/<n>`, base `origin/main` (`44d0d9d`, which `v2` started from) or the branch in `prs.tsv` for stacked ones |
| The pin | `3140648a668aaebf87bae2dea17512fdca125ba0` |
| `v2`'s core as ported so far | `/Volumes/Hangar/ps5/core-v2`, branch `horizon-v2`: the pin, the 17 EglantinePS5 patches as commits, then the delta units being cherry-picked. Another agent commits there: read it only (`git -C … log/show/diff/merge-tree`), never check out, fetch, reset or write |
| `v2` | `/Volumes/Hangar/ps5/horizon-ps5/.claude/worktrees/v2`: read only, except your own file here |

A pull request's change is `git diff $(git merge-base <base> <head>) <head>`. Its base is usually newer than the pin, so it may build on post-pin units: name them in Needs, with their verdict from `../delta/units.tsv`.

## How to judge one

1. Skip in one line what the PS5 never builds: only `src/yuzu`, `src/qt_common`, `src/android`, `src/yuzu_cmd`, `.github`, docs, Windows, MinGW, MSVC, macOS, MoltenVK, Discord, Nix, desktop input (DualSense rumble, Pro Controller motion), desktop HDR output.
2. Already there? Compare `git patch-id --stable` of its commits with the pin's history and with the commits of the delta units. Check whether `horizon-v2` already holds the same change (`git -C /Volumes/Hangar/ps5/core-v2 log -S'<distinctive line>' horizon-v2`).
3. Does it apply? For a core pull request, `git merge-tree --write-tree --merge-base=<its base> <horizon-v2 tip> <head>` in a scratch clone made with `git clone -q --no-checkout /Volumes/Hangar/ps5/core-v2 <your scratch>/core` (then fetch the head into it from the analysis clone). It says whether the change conflicts with what `v2` has now. Name the conflicting files in the Why.
4. Where does the PS5 port touch the same code? `rg -n '<path>' headless/CMakeLists.txt headless/*.cmake tools/prepare-vulkan-port.py` in the `v2` worktree, and the patch commits on `horizon-v2` (subjects `chore(core): EglantinePS5 patch 00NN …`). Write `anchors: <file>`, `cmake: <file>` or `patch 00NN` in the Why.
5. Value: the owner's games are Mario Kart World, Donkey Kong Bananza, Pokémon Pokopia, Zelda: Ocarina of Time, Kirby Air Riders, Mario Tennis Fever and The Duskbloods. On the PS5, CPU time in the x86-64 JIT, the 12 GiB shared pool and RADV correctness matter most. A draft or a pull request whose author says it is untested is `measure` at most.

The Why is one or two sentences with the evidence: file and function, game, driver, measurement, and the conflict if any.
