# input

Units of the upstream delta whose largest share of changed lines is in HID and input (`src/hid_core/`, `src/input_common/`). Writer: the agent assigned to this file; nobody else edits it. Verdicts and how to read a unit: [README](../README.md).

| Unit | Date | Title | Size | Also touches | Diff | Verdict | Why |
|---|---|---|---|---|---|---|---|
| dj:c08864b3fe | 2026-10-31 | refactor: remove unused gamepad button and axis event handling in SDLDriver | +0 -16, 1 files, 1 commits | - | `1051d394d2..c08864b3fe` | skip | Only input_common/drivers/sdl_driver.cpp, which the PS5 build does not compile: eden-headless links core and eden-pad, whose input_common sources are input_engine.cpp, input_poller.cpp and virtual_gamepad.cpp beside headless/pad.cpp. |
| dj#296 | 2026-11-02 | hid: report non-zero sensor ranges in the six-axis IC information | +11 -1, 1 files, 1 commits | - | `c0cb3325e2..8c63450666` | port | hid_core's six_axis.h in the pin still defaults ic_information to zero, so GetSixAxisSensorIcInformation reports 0 ranges whatever pad the PS5 drives (headless/pad.cpp does not set it) and Nintendo Switch Sports aborts in GyroPointingStateDetector (2162-0001); the fix reports 2000 dps and 8 g; review: its commit 7c36bf54bf has the same patch as e7fe1ddba5 in dj#495 (services, port), so it comes to v2 once, with dj#495, as dj#297 does. |
| dj#338 | 2026-11-03 | input_common/sdl: only log motion capabilities when they change | +23 -3, 1 files, 1 commits | - | `5c8858ebdb..d75018d5e1` | skip | Only input_common/drivers/sdl_driver.cpp (motion capability logging), which the PS5 build does not compile: its pad is headless/pad.cpp on the virtual_gamepad engine (eden-pad in headless/CMakeLists.txt). |
