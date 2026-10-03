// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "core/sched/scheduler.h"

using namespace heno::core;

class MockComponent : public ISchedulable {
public:
    uint64_t caught_up_to = 0;
    int catch_up_calls = 0;

    void catch_up(uint64_t target_master_cycle) override {
        caught_up_to = target_master_cycle;
        catch_up_calls++;
    }
};

class ReentrantComponent : public ISchedulable {
public:
    Scheduler* sched;
    uint64_t caught_up_to = 0;
    int catch_up_calls = 0;

    ReentrantComponent(Scheduler* s) : sched(s) {}

    void catch_up(uint64_t target_master_cycle) override {
        caught_up_to = target_master_cycle;
        catch_up_calls++;
        
        // Simulate a bus access that internally advances the scheduler again
        sched->advance(6);
    }
};

TEST_CASE("Scheduler advance and catch_up", "[sched]") {
    Scheduler sched;
    MockComponent ppu;
    MockComponent apu;

    sched.register_component(ComponentID::PPU, &ppu);
    sched.register_component(ComponentID::APU, &apu);

    sched.advance(6);

    REQUIRE(sched.current_cycle() == 6);
    REQUIRE(ppu.caught_up_to == 6);
    REQUIRE(apu.caught_up_to == 6);
    REQUIRE(ppu.catch_up_calls == 1);
    REQUIRE(apu.catch_up_calls == 1);
}

TEST_CASE("Scheduler prevents reentrancy", "[sched]") {
    Scheduler sched;
    ReentrantComponent ppu(&sched);
    MockComponent apu;

    sched.register_component(ComponentID::PPU, &ppu);
    sched.register_component(ComponentID::APU, &apu);

    sched.advance(8);

    // The advance(8) triggers PPU catch_up(8).
    // Inside PPU catch_up, it calls advance(6).
    // Because reentrancy is prevented, that advance(6) simply adds 6 to current_cycle,
    // but does NOT trigger another round of catch_up calls immediately.
    
    REQUIRE(sched.current_cycle() == 14); // 8 + 6
    REQUIRE(ppu.catch_up_calls == 1);     // Only called once!
    REQUIRE(ppu.caught_up_to == 8);
    
    // APU gets the first catch_up call. Note: the order might depend on registration
    // or iteration order. APU is id 2, PPU is id 1. PPU is called first.
    // Since PPU's catch_up bumped the cycle to 14, when APU's catch_up is called 
    // within the SAME loop, does it see 14? Yes, because current_cycle was updated.
    // Actually, catch_up(m_current_cycle) will pass 14 to APU! This might be surprising 
    // but it's consistent with preventing reentrancy and avoiding nested loops.
    REQUIRE(apu.catch_up_calls == 1);
    REQUIRE(apu.caught_up_to == 14);
}
