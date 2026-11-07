// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>

#include "video_core/semaphore_ledger.h"

namespace {
constexpr u64 Word = 0x8'0000'1040;
constexpr u64 OtherWord = Word + 4;
} // Anonymous namespace

TEST_CASE("A release still queued for the back answers the acquire without a drain",
          "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    ledger.NoteRelease(Word, 7, 100);
    REQUIRE(ledger.QueuedValue(Word, 0) == 7u);
    REQUIRE(ledger.QueuedValue(Word, 99) == 7u);
}

TEST_CASE("A release the back has run leaves the acquire to the guest word or the drain",
          "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    ledger.NoteRelease(Word, 7, 100);
    REQUIRE_FALSE(ledger.QueuedValue(Word, 100).has_value());
    REQUIRE_FALSE(ledger.QueuedValue(Word, 4096).has_value());
}

TEST_CASE("The last release queued for a word decides, so an acquire it does not meet drains",
          "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    ledger.NoteRelease(Word, 9, 100);
    ledger.NoteRelease(Word, 3, 120);
    const auto queued = ledger.QueuedValue(Word, 110);
    REQUIRE(queued == 3u);
    REQUIRE_FALSE(*queued >= 9u);
    REQUIRE_FALSE(ledger.QueuedValue(Word, 120).has_value());
}

TEST_CASE("Words without a queued release have no answer",
          "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    REQUIRE_FALSE(ledger.QueuedValue(Word, 0).has_value());
    ledger.NoteRelease(Word, 7, 100);
    REQUIRE_FALSE(ledger.QueuedValue(OtherWord, 0).has_value());
}

TEST_CASE("A word the front cannot follow is forgotten", "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    ledger.NoteRelease(Word, 7, 100);
    ledger.NoteRelease(OtherWord, 5, 100);
    ledger.Forget(Word);
    REQUIRE_FALSE(ledger.QueuedValue(Word, 0).has_value());
    REQUIRE(ledger.QueuedValue(OtherWord, 0) == 5u);
}

TEST_CASE("A stopped split drops the ring positions of its session",
          "[video_core][nx2_semaphore_ledger]") {
    Tegra::Nx2SemaphoreLedger ledger;
    ledger.NoteRelease(Word, 7, 1'000'000);
    ledger.NoteRelease(OtherWord, 5, 1'000'000);
    ledger.Clear();
    REQUIRE_FALSE(ledger.QueuedValue(Word, 0).has_value());
    REQUIRE_FALSE(ledger.QueuedValue(OtherWord, 0).has_value());
}
