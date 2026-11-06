// SPDX-FileCopyrightText: Copyright 2026 Eglantine Contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <utility>

#include "common/polyfill_thread.h"

namespace Common {

// Publishing host work must not wait for the consumer to execute earlier work.
// Producers can hold a memory-registry lock that the GPU consumer needs before
// it can finish its current command. A bounded queue creates a lock cycle there.
template <typename T>
class HostCommandQueue {
public:
    template <typename... Args>
    void Emplace(Args&&... args) {
        {
            std::scoped_lock lock{mutex};
            entries.emplace_back(std::forward<Args>(args)...);
        }
        available.notify_one();
    }

    bool TryPop(T& out) {
        std::scoped_lock lock{mutex};
        if (entries.empty()) {
            return false;
        }
        out = std::move(entries.front());
        entries.pop_front();
        return true;
    }

    void PopWait(T& out, std::stop_token stop) {
        std::unique_lock lock{mutex};
        if (!available.wait(lock, stop, [this] { return !entries.empty(); })) {
            return;
        }
        out = std::move(entries.front());
        entries.pop_front();
    }

private:
    std::mutex mutex;
    std::condition_variable_any available;
    std::deque<T> entries;
};

} // namespace Common
