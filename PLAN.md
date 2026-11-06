# v2

`main` is EglantinePS5 as published (heydemoura/EglantinePS5 `44d0d9d`). `v2` is `main` plus the upstream Eglantine and Lichii work that helps on the PS5, each change measured against `main`. The triage lives in [agents/](agents/README.md).

## Stages

| Stage | Goal | Done when | Status |
|---|---|---|---|
| 0 | Build `main` on the Air, in Docker, everything on Hangar | `dist/EglantinePS5-v*.zip` exists | in progress (2026-11-05 22:29): clean build in an amd64 image under Rosetta. The build assumes an x86-64 Linux host: on arm64 the RADV isolation (GNU binutils), the link (clang's x86-64 builtins) and the post-link checks (x86-only port code built for the host) all failed |
| 1 | Triage the 503 upstream units (666 commits) | every row in `agents/delta/*.md` has a verdict and a why, reviewed | done (2026-11-05 23:25): 105 port, 31 measure, 50 depends, 87 in port, 230 skip after 4 independent reviews; cross-file findings in `agents/findings.md` |
| 2 | The emulator becomes part of this repository | `Eglantine/` is a folder of this repository at the pin, the 17 patches are commits, and the synced source equals `main`'s | done (2026-11-05 23:27): `HEAD:Eglantine` has the pin's tree hash after the import and the work clone's tree hash after the 17 patch commits, and that tree equals `main`'s patched submodule (`diff -rq`) |
| 3 | Port the `port` units to `v2` | each unit's commits cherry-picked with their authors (`-x`), deferred ones logged; Android and Qt removed last; `make` and `make test` pass | in progress (2026-11-05 23:27): porting agent on `/Volumes/Hangar/ps5/core-v2` (`horizon-v2`), state in [agents/port-log.md](agents/port-log.md) |
| 4 | A/B on the console | `tools/mkw-bench-console.py` report for `main` and `v2`, interleaved, warm caches, one shader cache per build; each `measure` unit kept or dropped by its numbers | needs the PS5 |
| 5 | Own architecture | the build-time rewrites (`prepare-vulkan-port.py`, the CMake `string(REPLACE)` blocks) are source changes, checked byte for byte against what the old pipeline generated; then a PS5 platform layer | after stage 4 |
| 6 | Publish `v2` | `v2` pushed with its results | not started |

## Build

The image is `horizon-ps5-build-amd64` (`/Volumes/Hangar/ps5/docker/`: Ubuntu 24.04 amd64, run through OrbStack's Rosetta, the repository's `tools/ci/setup-ubuntu-24.04.sh`, and meson 1.10.1 installed system-wide). It has to be amd64: the build runs checks that compile x86-64-only port code for the build host, as the upstream CI's x86-64 runners do. Home, ccache and logs stay on Hangar:

```console
$ docker run --rm -t --platform linux/amd64 --name horizon-ps5-make -v /Volumes/Hangar/ps5:/Volumes/Hangar/ps5 \
    -e HOME=/Volumes/Hangar/ps5/home-amd64 -e CCACHE_DIR=/Volumes/Hangar/ps5/home-amd64/.ccache -e CCACHE_MAXSIZE=8G \
    -w /Volumes/Hangar/ps5/horizon-ps5 horizon-ps5-build-amd64 make JOBS=6 RADV_BUILD_JOBS=6
```

Logs: `/Volumes/Hangar/ps5/logs/make-<date>-<time>.log`.
