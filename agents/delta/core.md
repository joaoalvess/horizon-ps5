# core

Units of the upstream delta whose largest share of changed lines is in the rest of src/core (`src/core/`). Writer: the agent assigned to this file; nobody else edits it. Verdicts and how to read a unit: [README](../README.md).

| Unit | Date | Title | Size | Also touches | Diff | Verdict | Why |
|---|---|---|---|---|---|---|---|
| dj#327 | 2026-11-03 | tests when building on Windows: compare FindBestUpdatePackage with a size_t | +1 -1, 1 files, 1 commits | - | `8362c44ff8..8c6539b564` | skip | Only src/tests/core/nup_writer.cpp (a Windows sign-compare warning), which the PS5 build does not compile (YUZU_TESTS=OFF, BUILD_TESTING=OFF in tools/build-headless-native.sh). |
| dj:b1e5c7ec68 | 2026-11-03 | tests: compare the socket receive count without a sign mismatch | +1 -1, 1 files, 1 commits | - | `cf55e9e9f4..b1e5c7ec68` | skip | Only src/tests/core/internal_network/socket_proxy.cpp (a sign-compare warning), which the PS5 build does not compile (YUZU_TESTS=OFF, BUILD_TESTING=OFF in tools/build-headless-native.sh). |
