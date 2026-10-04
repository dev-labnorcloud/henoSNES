// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "bus/bus.h"
#include "bus/io_device.h"
#include "cart/cartridge.h"

using namespace heno;

struct MockScheduler {
    std::uint32_t total_cycles = 0;
    void advance(std::uint32_t cycles) {
        total_cycles += cycles;
    }
};
static_assert(ClockSink<MockScheduler>);

struct HookContext {
    std::uint32_t last_addr = 0;
    std::uint8_t last_value = 0;
    bool last_is_write = false;
    int calls = 0;
};

void test_hook(void* ctx, std::uint32_t addr, std::uint8_t value, bool is_write) {
    auto* hc = static_cast<HookContext*>(ctx);
    hc->last_addr = addr;
    hc->last_value = value;
    hc->last_is_write = is_write;
    hc->calls++;
}

TEST_CASE("Bus Behavior", "[bus]") {
    std::vector<std::uint8_t> rom(65536, 0xAA);
    Cartridge cart(rom, MapMode::LoRom);
    MockScheduler sched;
    NullIo io;
    Bus<MockScheduler, NullIo> bus(sched, cart, io);
    
    SECTION("read8 in ROM advances cycles and returns value") {
        std::uint8_t val = bus.read8(0x008000);
        REQUIRE(val == 0xAA);
        REQUIRE(sched.total_cycles == 8); // Bank 00 is slowROM
    }
    
    SECTION("read8 in unmapped region returns last open bus value") {
        // Read mapped region first
        bus.read8(0x008000); // returns 0xAA, open bus is now 0xAA
        REQUIRE(bus.state().open_bus == 0xAA);
        
        // Read unmapped
        std::uint8_t val = bus.read8(0x002000); // 0x002000 not mapped in cart nor wram
        REQUIRE(val == 0xAA);
        REQUIRE(sched.total_cycles == 14); // 8 from first + 6 from second (0x2000 is 6 cycles)
    }
    
    SECTION("write8 to $420D toggles FastROM") {
        bus.write8(0x00420D, 1);
        REQUIRE(bus.state().fastrom == true);
        REQUIRE(sched.total_cycles == 6); // 420D is 6 cycles
        
        std::uint32_t cycles_before = sched.total_cycles;
        bus.read8(0x808000);
        REQUIRE(sched.total_cycles - cycles_before == 6); // FastROM
        
        bus.write8(0x00420D, 0);
        REQUIRE(bus.state().fastrom == false);
        
        cycles_before = sched.total_cycles;
        bus.read8(0x808000);
        REQUIRE(sched.total_cycles - cycles_before == 8); // SlowROM again
    }
    
    SECTION("write8 in ROM doesn't change next read") {
        bus.read8(0x008000); // Load open_bus with 0xAA
        bus.write8(0x008000, 0x55); // Try to write to ROM
        REQUIRE(bus.state().open_bus == 0x55); // open bus captures the written value
        
        std::uint8_t val = bus.read8(0x008000);
        REQUIRE(val == 0xAA); // Cartridge read still returns 0xAA, rewriting open_bus
    }
    
    SECTION("idle advances 6 cycles") {
        bus.idle();
        REQUIRE(sched.total_cycles == 6);
    }
    
    SECTION("Hook is called") {
        HookContext hc;
        bus.set_hook(test_hook, &hc);
        
        bus.read8(0x008000);
        REQUIRE(hc.calls == 1);
        REQUIRE(hc.last_addr == 0x008000);
        REQUIRE(hc.last_value == 0xAA);
        REQUIRE(hc.last_is_write == false);
        
        bus.write8(0x00420D, 0x01);
        REQUIRE(hc.calls == 2);
        REQUIRE(hc.last_addr == 0x00420D);
        REQUIRE(hc.last_value == 0x01);
        REQUIRE(hc.last_is_write == true);
    }
    
    SECTION("State load/save") {
        BusState s;
        s.open_bus = 0xBB;
        s.fastrom = true;
        bus.load_state(s);
        
        REQUIRE(bus.state().open_bus == 0xBB);
        REQUIRE(bus.state().fastrom == true);
    }
}

struct MockIoDevice {
    std::vector<std::uint32_t> accesses;
    bool return_value = true;
    std::uint8_t read_value = 0;

    bool read(std::uint32_t addr, std::uint8_t /*open_bus*/, std::uint8_t& out) noexcept {
        accesses.push_back(addr);
        if (return_value) {
            out = read_value;
            return true;
        }
        return false;
    }

    bool write(std::uint32_t addr, std::uint8_t /*value*/) noexcept {
        accesses.push_back(addr);
        return return_value;
    }
};
static_assert(IoDevice<MockIoDevice>);

TEST_CASE("E/S Routing", "[bus][io]") {
    std::vector<std::uint8_t> rom(65536, 0x00);
    Cartridge cart(rom, MapMode::LoRom);
    MockScheduler sched;
    MockIoDevice io;
    Bus<MockScheduler, MockIoDevice> bus(sched, cart, io);

    SECTION("Accesos enrutados al dispositivo de E/S") {
        std::vector<std::uint32_t> targets = {
            0x002100, 0x00217F, 0x002184, 0x0021FF, 0x004000, 0x004016, 0x004300
        };
        for (auto addr : targets) {
            bus.read8(addr);
            REQUIRE(io.accesses.back() == addr);
            bus.write8(addr, 0xFF);
            REQUIRE(io.accesses.back() == addr);
        }
    }

    SECTION("Accesos no enrutados al dispositivo de E/S") {
        std::vector<std::uint32_t> targets = {
            0x002180, 0x002181, 0x002182, 0x002183, 0x00420D
        };
        for (auto addr : targets) {
            bus.read8(addr);
            bus.write8(addr, 0xFF);
        }
        REQUIRE(io.accesses.empty());
    }

    SECTION("Dispositivo devuelve false -> open bus") {
        io.return_value = false;
        bus.write8(0x002000, 0xAB); // set open bus to 0xAB
        std::uint8_t val = bus.read8(0x002100); // routed to io, but returns false
        REQUIRE(val == 0xAB);
    }

    SECTION("Lectura de WRAM actualiza open_bus") {
        bus.write8(0x7E0000, 0xCD); // escribe en WRAM
        // escribe otra cosa a un lugar unmapped para cambiar open_bus
        bus.write8(0x002000, 0x00); 
        REQUIRE(bus.state().open_bus == 0x00);

        // lee WRAM
        std::uint8_t val = bus.read8(0x7E0000);
        REQUIRE(val == 0xCD);
        REQUIRE(bus.state().open_bus == 0xCD); // open bus actualizado
    }
}
