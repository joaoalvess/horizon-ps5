// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#include "common/common_types.h"

namespace Vulkan::Nx2DrawRing {

enum class Kind : u32 {
    Draw,
    DrawIndexed,
    Mesh,
    DrawIndirect,
    DrawIndexedIndirect,
    Dispatch,
    DispatchIndirect,
};

struct Entry {
    u64 tick;
    Kind kind;
    std::array<u64, 3> programs;
    std::array<u32, 8> values;
};

inline constexpr u64 Capacity = 16384;
inline constexpr u64 LargeDirectDraw = u64{1} << 20;
inline constexpr u32 MaxDumps = 8;

struct State {
    std::mutex mutex;
    std::array<Entry, Capacity> entries;
    u64 added{};
    u32 dumps{};
};

inline State& GetState() {
    static State state;
    return state;
}

inline bool Enabled() {
    static const bool enabled = std::getenv("EDEN_NX2_DRAW_RING") != nullptr;
    return enabled;
}

inline void Add(const Entry& entry) {
    State& state{GetState()};
    std::scoped_lock lock{state.mutex};
    state.entries[state.added++ % Capacity] = entry;
}

inline void Dump(u64 waited_tick, u64 gpu_tick, u64 blocked_ms) {
    static constexpr std::array names{
        "draw",          "draw-indexed", "mesh", "draw-indirect", "draw-indexed-indirect",
        "dispatch",      "dispatch-indirect",
    };
    State& state{GetState()};
    std::scoped_lock lock{state.mutex};
    if (state.dumps++ >= MaxDumps) {
        return;
    }
    std::fprintf(stderr,
                 "NX2_DRAW_RING blocked_ms=%llu waited_tick=%llu gpu_tick=%llu recorded=%llu\n",
                 static_cast<unsigned long long>(blocked_ms),
                 static_cast<unsigned long long>(waited_tick),
                 static_cast<unsigned long long>(gpu_tick),
                 static_cast<unsigned long long>(state.added));
    u64 small_direct_draws{};
    for (u64 index = state.added - std::min(state.added, Capacity); index < state.added; ++index) {
        const Entry& entry{state.entries[index % Capacity]};
        if (entry.tick <= gpu_tick) {
            continue;
        }
        const bool direct = entry.kind == Kind::Draw || entry.kind == Kind::DrawIndexed;
        if (direct && u64{entry.values[0]} * entry.values[1] < LargeDirectDraw) {
            ++small_direct_draws;
            continue;
        }
        std::fprintf(stderr,
                     "NX2_DRAW_RING tick=%llu %s programs=%016llx,%016llx,%016llx "
                     "values=%u,%u,%u,%u,%u,%u,%u,%u\n",
                     static_cast<unsigned long long>(entry.tick),
                     names[static_cast<u32>(entry.kind)],
                     static_cast<unsigned long long>(entry.programs[0]),
                     static_cast<unsigned long long>(entry.programs[1]),
                     static_cast<unsigned long long>(entry.programs[2]), entry.values[0],
                     entry.values[1], entry.values[2], entry.values[3], entry.values[4],
                     entry.values[5], entry.values[6], entry.values[7]);
    }
    std::fprintf(stderr, "NX2_DRAW_RING end small_direct_draws=%llu\n",
                 static_cast<unsigned long long>(small_direct_draws));
    std::fflush(stderr);
}

} // namespace Vulkan::Nx2DrawRing
