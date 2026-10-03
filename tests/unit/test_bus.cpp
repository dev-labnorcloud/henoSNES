// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "core/bus/bus.h"

using namespace heno::core;

TEST_CASE("Bus cycles and MEMSEL", "[bus]") {
    std::vector<uint8_t> rom(0x10000, 0);
    Cartridge cart(rom);
    Scheduler sched;
    Bus bus(sched, cart);

    // Initial state: SlowROM
    bus.read8(0x008000); // SlowROM, bank 00
    REQUIRE(sched.current_cycle() == 8);
    
    bus.read8(0x808000); // Still SlowROM, bank 80
    REQUIRE(sched.current_cycle() == 16);

    // Enable FastROM
    bus.write8(0x420D, 0x01); // Takes 6 cycles
    REQUIRE(sched.current_cycle() == 22);

    bus.read8(0x808000); // FastROM, bank 80
    REQUIRE(sched.current_cycle() == 28); // +6

    bus.read8(0x008000); // Still SlowROM, bank 00
    REQUIRE(sched.current_cycle() == 36); // +8
    
    bus.read8(0xC00000); // FastROM, bank C0
    REQUIRE(sched.current_cycle() == 42); // +6
    
    bus.write8(0x4000, 0x00); // Old Joypad
    REQUIRE(sched.current_cycle() == 54); // +12
}

TEST_CASE("Bus WRAM Mirrors", "[bus]") {
    std::vector<uint8_t> rom(0x10000, 0);
    Cartridge cart(rom);
    Scheduler sched;
    Bus bus(sched, cart);

    // Write to WRAM $7E:0050
    bus.write8(0x7E0050, 0x42);
    
    // Read from Mirror $00:0050
    uint8_t val1 = bus.read8(0x000050);
    REQUIRE(val1 == 0x42);
    
    // Read from Mirror $80:0050
    uint8_t val2 = bus.read8(0x800050);
    REQUIRE(val2 == 0x42);
    
    // Read from WRAM $7F:0050 (not mirrored to 0050, it's 64K higher)
    uint8_t val3 = bus.read8(0x7F0050);
    REQUIRE(val3 == 0x00);
}

TEST_CASE("Bus Open Bus", "[bus]") {
    std::vector<uint8_t> rom(0x10000, 0);
    Cartridge cart(rom);
    Scheduler sched;
    Bus bus(sched, cart);

    bus.write8(0x7E0000, 0xAA); 
    
    uint8_t val = bus.read8(0x002100); 
    REQUIRE(val == 0xAA);
    
    bus.write8(0x7E0000, 0x55); 
    val = bus.read8(0x002100);
    REQUIRE(val == 0x55);
}
