// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <stop_token>

namespace Common {

// One synchronous command at a time per submitting host thread. A queued command keeps
// its own reference through Signal(), even if Wait() returns and the caller exits.
class HostCommandCompletion final {
public:
    static std::shared_ptr<HostCommandCompletion> ForCurrentThread() {
        thread_local auto completion = std::make_shared<HostCommandCompletion>();
        if (!completion->Reusable()) completion = std::make_shared<HostCommandCompletion>();
        completion->Reset();
        return completion;
    }

    void Wait() {
        std::unique_lock lock{mutex};
        ready.wait(lock, [this] { return done; });
    }
    bool Wait(std::stop_token stop) {
        std::unique_lock lock{mutex};
        if (ready.wait(lock, stop, [this] { return done; })) return true;
        reusable = false; // A late callback must never complete a later request.
        return false;
    }
    void Signal() {
        std::lock_guard lock{mutex};
        done = true;
        ready.notify_one();
    }

private:
    bool Reusable() {
        std::lock_guard lock{mutex};
        return reusable;
    }
    void Reset() {
        std::lock_guard lock{mutex};
        done = false;
    }
    std::mutex mutex;
    std::condition_variable_any ready;
    bool done{};
    bool reusable{true};
};

} // namespace Common
