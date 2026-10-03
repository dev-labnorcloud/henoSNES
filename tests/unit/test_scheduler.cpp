// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "sched/scheduler.h"
#include <vector>

struct Call {
    enum Type { Ppu, Apu };
    Type type;
    std::uint64_t target;
};

struct MockPpu {
    std::vector<Call>* calls;
    void catch_up(std::uint64_t target) {
        calls->push_back({Call::Ppu, target});
    }
};

struct MockApu {
    std::vector<Call>* calls;
    void catch_up(std::uint64_t target) {
        calls->push_back({Call::Apu, target});
    }
};

using namespace heno;

TEST_CASE("Accuracy policy: advance calls catch_up in order", "[scheduler]") {
    std::vector<Call> calls;
    MockPpu ppu{&calls};
    MockApu apu{&calls};
    Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);

    sched.advance(10);

    REQUIRE(calls.size() == 2);
    CHECK(calls[0].type == Call::Ppu);
    CHECK(calls[0].target == 10);
    CHECK(calls[1].type == Call::Apu);
    CHECK(calls[1].target == (10ULL * kApuRatioNtsc.num) / kApuRatioNtsc.den);
}

TEST_CASE("Accuracy policy: sync_all after advance produces no calls", "[scheduler]") {
    std::vector<Call> calls;
    MockPpu ppu{&calls};
    MockApu apu{&calls};
    Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);

    sched.advance(10);
    calls.clear();

    sched.sync_all();
    CHECK(calls.empty());
}

TEST_CASE("Clock conversion exactness", "[scheduler]") {
    std::vector<Call> calls;
    MockPpu ppu{&calls};
    MockApu apu{&calls};
    Scheduler<SyncPolicy::Performance, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);

    // Sum mixed chunks totaling 109375
    sched.advance(1);
    sched.advance(6);
    sched.advance(8);
    sched.advance(12);
    sched.advance(109375 - 1 - 6 - 8 - 12);

    auto state = sched.state();
    CHECK(state.master_cycle == 109375);
    CHECK(state.apu_cycle == 125312);
    CHECK(state.apu_acc == 0);
}

TEST_CASE("Clock conversion exactness for multiple N", "[scheduler]") {
    std::vector<Call> calls;
    MockPpu ppu{&calls};
    MockApu apu{&calls};

    for (int N : {1, 2, 5, 10, 100}) {
        Scheduler<SyncPolicy::Performance, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);
        sched.advance(N * 109375);
        auto state = sched.state();
        CHECK(state.apu_cycle == static_cast<std::uint64_t>(N) * 125312);
        CHECK(state.apu_acc == 0);
    }
}

TEST_CASE("Performance policy: lazy sync", "[scheduler]") {
    std::vector<Call> calls;
    MockPpu ppu{&calls};
    MockApu apu{&calls};
    Scheduler<SyncPolicy::Performance, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);

    sched.advance(100);
    CHECK(calls.empty());

    sched.sync_ppu();
    REQUIRE(calls.size() == 1);
    CHECK(calls[0].type == Call::Ppu);
    CHECK(calls[0].target == 100);

    sched.sync_apu();
    REQUIRE(calls.size() == 2);
    CHECK(calls[1].type == Call::Apu);
    CHECK(calls[1].target == (100ULL * kApuRatioNtsc.num) / kApuRatioNtsc.den);

    // sync_all without advance should not produce calls
    calls.clear();
    sched.sync_all();
    CHECK(calls.empty());
}

TEST_CASE("State copy and load", "[scheduler]") {
    std::vector<Call> calls1;
    MockPpu ppu1{&calls1};
    MockApu apu1{&calls1};
    Scheduler<SyncPolicy::Performance, MockPpu, MockApu> sched1(ppu1, apu1, kApuRatioNtsc);

    sched1.advance(150);
    auto state = sched1.state();

    std::vector<Call> calls2;
    MockPpu ppu2{&calls2};
    MockApu apu2{&calls2};
    Scheduler<SyncPolicy::Performance, MockPpu, MockApu> sched2(ppu2, apu2, kApuRatioNtsc);

    sched2.load_state(state);

    // Check identical states
    CHECK(sched2.state().master_cycle == state.master_cycle);
    CHECK(sched2.state().apu_cycle == state.apu_cycle);
    CHECK(sched2.state().apu_acc == state.apu_acc);

    // sync_all after load_state produces no calls
    sched2.sync_all();
    CHECK(calls2.empty());
}

TEST_CASE("Determinism", "[scheduler]") {
    std::vector<Call> calls1;
    MockPpu ppu1{&calls1};
    MockApu apu1{&calls1};
    Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu> sched1(ppu1, apu1, kApuRatioNtsc);

    std::vector<Call> calls2;
    MockPpu ppu2{&calls2};
    MockApu apu2{&calls2};
    Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu> sched2(ppu2, apu2, kApuRatioNtsc);

    std::vector<std::uint32_t> advances = {6, 8, 12, 1, 100, 50, 4};
    for (auto adv : advances) {
        sched1.advance(adv);
        sched2.advance(adv);
    }

    auto s1 = sched1.state();
    auto s2 = sched2.state();

    CHECK(s1.master_cycle == s2.master_cycle);
    CHECK(s1.apu_cycle == s2.apu_cycle);
    CHECK(s1.apu_acc == s2.apu_acc);
}
