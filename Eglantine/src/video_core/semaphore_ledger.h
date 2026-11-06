// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdlib>
#include <optional>
#include <unordered_map>

#include "common/common_types.h"

namespace Tegra {

class Nx2SemaphoreLedger {
public:
    struct Entry {
        u64 order;
        u32 value;
    };

    static Nx2SemaphoreLedger& Get() {
        static Nx2SemaphoreLedger ledger;
        return ledger;
    }

    static bool Enabled() {
        static const bool enabled = [] {
            const char* value = std::getenv("EDEN_NX2_ACQ_LEDGER");
            return value != nullptr && value[0] == '1';
        }();
        return enabled;
    }

    void NoteRelease(u64 device_address, u32 value, u64 order) {
        entries.insert_or_assign(device_address, Entry{order, value});
    }

    void Forget(u64 device_address) {
        entries.erase(device_address);
    }

    void Clear() {
        entries.clear();
    }

    [[nodiscard]] std::optional<u32> QueuedValue(u64 device_address, u64 consumed) {
        const auto it = entries.find(device_address);
        if (it == entries.end()) {
            return std::nullopt;
        }
        if (it->second.order <= consumed) {
            entries.erase(it);
            return std::nullopt;
        }
        return it->second.value;
    }

private:
    std::unordered_map<u64, Entry> entries;
};

} // namespace Tegra
