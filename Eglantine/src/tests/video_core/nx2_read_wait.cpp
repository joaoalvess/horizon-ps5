// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <chrono>
#include <thread>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "video_core/nx2.h"

namespace {

using Span = std::pair<u64, u64>;

struct Ranges {
    std::vector<Span> spans;

    template <typename Func>
    void ForEachInRange(u64 address, u64 size, Func&& func) const {
        const u64 end = address + size;
        for (const auto& [begin, last] : spans) {
            if (begin < end && address < last) {
                func((std::max)(begin, address), (std::min)(last, end));
            }
        }
    }
};

Tegra::Nx2ReadDependency Classify(u64 address, u64 size, u64 popped, const Ranges& gpu_modified,
                                  const std::vector<Ranges>& pending) {
    return Tegra::Nx2BufferReadDependency(
        address, size, popped,
        [&](u64 begin, u64 bytes, auto&& modified) {
            gpu_modified.ForEachInRange(begin, bytes, modified);
        },
        [&](u64 begin, u64 bytes, auto&& covered) {
            for (u64 index = 0; index < pending.size(); ++index) {
                pending[index].ForEachInRange(begin, bytes, [&](u64 start, u64 end) {
                    covered(index, start, end);
                });
            }
        });
}

Tegra::Nx2ReadDependency Wait(Tegra::Nx2ReadWait wait, u64 downloads = 0, u64 trims = 0) {
    return {wait, downloads, trims};
}

}

TEST_CASE("A read of bytes the GPU has not written waits for nothing",
          "[video_core][nx2_read_wait]") {
    const Ranges gpu_modified{{{0x3000, 0x4000}}};
    const std::vector<Ranges> pending{Ranges{{{0x3000, 0x4000}}}};
    const auto dependency = Classify(0x1000, 0x1000, 5, gpu_modified, pending);
    REQUIRE(dependency.wait == Tegra::Nx2ReadWait::None);
    REQUIRE(dependency.downloads == 0);
}

TEST_CASE("A read waits only for the download that covers it, not for later ones",
          "[video_core][nx2_read_wait]") {
    const Ranges gpu_modified{{{0x1000, 0x2000}, {0x5000, 0x6000}}};
    const std::vector<Ranges> pending{Ranges{{{0x1000, 0x2000}}}, Ranges{{{0x5000, 0x6000}}}};
    const auto dependency = Classify(0x1000, 0x1000, 7, gpu_modified, pending);
    REQUIRE(dependency.wait == Tegra::Nx2ReadWait::Downloads);
    REQUIRE(dependency.downloads == 8);
}

TEST_CASE("A read covered by several downloads waits for the last of them",
          "[video_core][nx2_read_wait]") {
    const Ranges gpu_modified{{{0x1000, 0x2000}}};
    const std::vector<Ranges> pending{Ranges{{{0x1000, 0x1800}}}, Ranges{{{0x8000, 0x9000}}},
                                      Ranges{{{0x1800, 0x2000}}}};
    const auto dependency = Classify(0x1000, 0x1000, 2, gpu_modified, pending);
    REQUIRE(dependency.wait == Tegra::Nx2ReadWait::Downloads);
    REQUIRE(dependency.downloads == 5);
}

TEST_CASE("GPU-written bytes no download covers need the ordered read",
          "[video_core][nx2_read_wait]") {
    const Ranges gpu_modified{{{0x1000, 0x2000}}};
    SECTION("no download at all") {
        REQUIRE(Classify(0x1000, 0x1000, 0, gpu_modified, {}).wait ==
                Tegra::Nx2ReadWait::Ordered);
    }
    SECTION("a download that misses the last bytes") {
        const std::vector<Ranges> pending{Ranges{{{0x1000, 0x1ff0}}}};
        REQUIRE(Classify(0x1000, 0x1000, 0, gpu_modified, pending).wait ==
                Tegra::Nx2ReadWait::Ordered);
    }
    SECTION("downloads with a hole between them") {
        const std::vector<Ranges> pending{Ranges{{{0x1000, 0x1400}}}, Ranges{{{0x1500, 0x2000}}}};
        REQUIRE(Classify(0x1000, 0x1000, 0, gpu_modified, pending).wait ==
                Tegra::Nx2ReadWait::Ordered);
    }
}

TEST_CASE("One uncovered GPU-written range makes the whole read ordered, wherever it is",
          "[video_core][nx2_read_wait]") {
    const std::vector<Ranges> pending{Ranges{{{0x1000, 0x1100}}}};
    const Ranges uncovered_first{{{0x1200, 0x1300}, {0x1000, 0x1100}}};
    const Ranges uncovered_last{{{0x1000, 0x1100}, {0x1200, 0x1300}}};
    REQUIRE(Classify(0x1000, 0x1000, 0, uncovered_first, pending).wait ==
            Tegra::Nx2ReadWait::Ordered);
    REQUIRE(Classify(0x1000, 0x1000, 0, uncovered_last, pending).wait ==
            Tegra::Nx2ReadWait::Ordered);
}

TEST_CASE("Only the GPU-written bytes inside the read are looked at",
          "[video_core][nx2_read_wait]") {
    const Ranges gpu_modified{{{0x0, 0x1000}, {0x1800, 0x2800}}};
    const std::vector<Ranges> pending{Ranges{{{0x1800, 0x2000}}}};
    const auto dependency = Classify(0x1000, 0x1000, 0, gpu_modified, pending);
    REQUIRE(dependency.wait == Tegra::Nx2ReadWait::Downloads);
    REQUIRE(dependency.downloads == 1);
}

TEST_CASE("Download pops are counted and waited for", "[video_core][nx2_read_wait]") {
    Tegra::Nx2BufferDownloads downloads;
    REQUIRE(downloads.WaitPopped(0, std::chrono::milliseconds{0}));
    REQUIRE_FALSE(downloads.WaitPopped(1, std::chrono::milliseconds{1}));
    downloads.Pop();
    REQUIRE(downloads.Popped() == 1);
    REQUIRE(downloads.WaitPopped(1, std::chrono::milliseconds{0}));
    REQUIRE(downloads.Trims() == 0);
    downloads.Trim();
    REQUIRE(downloads.Trims() == 1);
}

TEST_CASE("A waiter wakes when the download it waits for pops on another thread",
          "[video_core][nx2_read_wait]") {
    Tegra::Nx2BufferDownloads downloads;
    bool popped = false;
    std::chrono::steady_clock::duration waited{};
    std::thread reader([&] {
        const auto begin = std::chrono::steady_clock::now();
        popped = downloads.WaitPopped(2, std::chrono::seconds{20});
        waited = std::chrono::steady_clock::now() - begin;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds{5});
    downloads.Pop();
    std::this_thread::sleep_for(std::chrono::milliseconds{5});
    downloads.Pop();
    reader.join();
    REQUIRE(popped);
    REQUIRE(waited < std::chrono::seconds{10});
}

TEST_CASE("A read with nothing pending or needing the ordered read does not wait",
          "[video_core][nx2_read_wait]") {
    const auto no_wait = [](u64) {
        FAIL("Nothing to wait for");
        return false;
    };
    const auto trims = [] { return u64{0}; };
    REQUIRE(Tegra::Nx2ReadAfterItsWriters([] { return Wait(Tegra::Nx2ReadWait::None); },
                                          no_wait, trims));
    REQUIRE_FALSE(Tegra::Nx2ReadAfterItsWriters(
        [] { return Wait(Tegra::Nx2ReadWait::Ordered); }, no_wait, trims));
}

TEST_CASE("A read covered by downloads is answered once they pop",
          "[video_core][nx2_read_wait]") {
    std::vector<u64> waits;
    REQUIRE(Tegra::Nx2ReadAfterItsWriters(
        [] { return Wait(Tegra::Nx2ReadWait::Downloads, 9, 3); },
        [&](u64 downloads) {
            waits.push_back(downloads);
            return true;
        },
        [] { return u64{3}; }));
    REQUIRE(waits == std::vector<u64>{9});
}

TEST_CASE("A download that does not pop in time sends the read to the ordered path",
          "[video_core][nx2_read_wait]") {
    REQUIRE_FALSE(Tegra::Nx2ReadAfterItsWriters(
        [] { return Wait(Tegra::Nx2ReadWait::Downloads, 4); }, [](u64) { return false; },
        [] { return u64{0}; }));
}

TEST_CASE("A write that took bytes out of a pending download makes the read look again",
          "[video_core][nx2_read_wait]") {
    u64 trims = 0;
    std::vector<Tegra::Nx2ReadDependency> answers;
    u32 classified = 0;
    const auto classify = [&] { return answers[classified++]; };
    const auto wait = [&](u64) {
        ++trims;
        return true;
    };
    const auto count = [&] { return trims; };
    SECTION("the bytes landed: answered") {
        answers = {Wait(Tegra::Nx2ReadWait::Downloads, 1, 0), Wait(Tegra::Nx2ReadWait::None)};
        REQUIRE(Tegra::Nx2ReadAfterItsWriters(classify, wait, count));
        REQUIRE(classified == 2);
    }
    SECTION("the write is not downloaded yet: ordered") {
        answers = {Wait(Tegra::Nx2ReadWait::Downloads, 1, 0), Wait(Tegra::Nx2ReadWait::Ordered)};
        REQUIRE_FALSE(Tegra::Nx2ReadAfterItsWriters(classify, wait, count));
        REQUIRE(classified == 2);
    }
    SECTION("the write's own download: waited for too") {
        answers = {Wait(Tegra::Nx2ReadWait::Downloads, 1, 0),
                   Wait(Tegra::Nx2ReadWait::Downloads, 2, 1)};
        u64 waited_for = 0;
        REQUIRE(Tegra::Nx2ReadAfterItsWriters(
            classify,
            [&](u64 downloads) {
                waited_for = downloads;
                if (downloads == 1) {
                    ++trims;
                }
                return true;
            },
            count));
        REQUIRE(waited_for == 2);
        REQUIRE(classified == 2);
    }
}

TEST_CASE("A read whose downloads keep being rewritten gives up and goes ordered",
          "[video_core][nx2_read_wait]") {
    u64 trims = 0;
    u32 classified = 0;
    REQUIRE_FALSE(Tegra::Nx2ReadAfterItsWriters(
        [&] {
            ++classified;
            return Wait(Tegra::Nx2ReadWait::Downloads, trims + 1, trims);
        },
        [&](u64) {
            ++trims;
            return true;
        },
        [&] { return trims; }));
    REQUIRE(classified == Tegra::Nx2ReadWaitAttempts);
}

TEST_CASE("Pages an image or a deferred query result holds are marked per page",
          "[video_core][nx2_read_wait]") {
    constexpr u64 Page = u64{1} << Tegra::Nx2WrittenPageBits;
    Tegra::Nx2ClearGpuWrittenPages();
    Tegra::Nx2MarkGpuHeld(16 * Page + Page - 8, 16);
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(15 * Page + Page - 1));
    REQUIRE(Tegra::Nx2IsGpuHeld(16 * Page));
    REQUIRE(Tegra::Nx2IsGpuHeld(17 * Page + Page - 1));
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(18 * Page));
    REQUIRE_FALSE(Tegra::Nx2IsGpuWritten(16 * Page));
    Tegra::Nx2MarkGpuHeld(63 * Page, 2 * Page);
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(62 * Page));
    REQUIRE(Tegra::Nx2IsGpuHeld(63 * Page));
    REQUIRE(Tegra::Nx2IsGpuHeld(64 * Page));
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(65 * Page));
    Tegra::Nx2MarkGpuHeld(100 * Page, 0);
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(100 * Page));
    Tegra::Nx2ClearGpuWrittenPages();
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(16 * Page));
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(64 * Page));
}

TEST_CASE("A held range past the device address space marks only its last page",
          "[video_core][nx2_read_wait]") {
    constexpr u64 Page = u64{1} << Tegra::Nx2WrittenPageBits;
    constexpr u64 AddressSpace = u64{1} << 36;
    Tegra::Nx2ClearGpuWrittenPages();
    Tegra::Nx2MarkGpuHeld(AddressSpace - Page, 4 * Page);
    REQUIRE(Tegra::Nx2IsGpuHeld(AddressSpace - Page));
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(AddressSpace));
    REQUIRE_FALSE(Tegra::Nx2IsGpuHeld(AddressSpace - 2 * Page));
    Tegra::Nx2ClearGpuWrittenPages();
}
