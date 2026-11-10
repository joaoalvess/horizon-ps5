# core

Units of the upstream delta whose largest share of changed lines is in the rest of src/core (`src/core/`). Writer: the agent assigned to this file; nobody else edits it. Verdicts and how to read a unit: [README](../README.md).

| Unit | Date | Title | Size | Also touches | Diff | Verdict | Why |
|---|---|---|---|---|---|---|---|
| dj#327 | 2026-11-03 | tests when building on Windows: compare FindBestUpdatePackage with a size_t | +1 -1, 1 files, 1 commits | - | `8362c44ff8..8c6539b564` | skip | Only src/tests/core/nup_writer.cpp (a Windows sign-compare warning), which the PS5 build does not compile (YUZU_TESTS=OFF, BUILD_TESTING=OFF in tools/build-headless-native.sh). |
| dj:b1e5c7ec68 | 2026-11-03 | tests: compare the socket receive count without a sign mismatch | +1 -1, 1 files, 1 commits | - | `cf55e9e9f4..b1e5c7ec68` | skip | Only src/tests/core/internal_network/socket_proxy.cpp (a sign-compare warning), which the PS5 build does not compile (YUZU_TESTS=OFF, BUILD_TESTING=OFF in tools/build-headless-native.sh). |
| dj:cc0fc78005 | 2026-11-06 | core: serialize GPU dirty memory gathers | +4 -0, 1 files, 1 commits | - | `53c1fa18b4..cc0fc78005` | in port | v2's gpu_dirty_memory_manager.h already holds gather_guard for the whole Gather (19d61164d6, li#138: merging this leaves v2's tree unchanged). |
| li#246 | 2026-11-07 | tests: compare unsigned results with unsigned literals | +4 -4, 2 files, 1 commits | gpu-core | `3420ca1345..e8d7907a34` | skip | Signedness of literals in two test files v2 does not have (nx2_debug_server.cpp, buffer_cache_ports.cpp). |
| li#182 | 2026-11-07 | debug server: frame_times for guest present intervals | +216 -6, 10 files, 1 commits | frontend-build, services, kernel-memory | `186d4dd1b8..1d7fe9b7d8` | skip | A frame_times tool for the desktop debug server (nx2_debug_tools.cpp, from li#111, skip), not in v2; same verdict as li-pr#182. |
| li#499 | 2026-11-07 | NUP: allow icon/title edits when metadata sits before the blob; take base-package icons | +380 -12, 4 files, 1 commits | loader-fs | `412c753b3a..1467051856` | skip | NUP metadata editing and base-package icons in nup_writer.cpp and upstream's loader/nup.cpp, neither of which v2 has; the PS5 reads NUP with its own loader (ps5 patch 0051, file_sys/nup.cpp and loader/nup.cpp) and never writes one. |
