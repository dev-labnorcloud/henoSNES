// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "bus/bus.h"
#include "bus/io_device.h"
#include "cart/cartridge.h"

using namespace heno;

struct DummyScheduler {
    void advance(std::uint32_t) {}
};
static_assert(ClockSink<DummyScheduler>);

TEST_CASE("WRAM Behavior", "[wram]") {
    std::vector<std::uint8_t> rom(65536, 0x00);
    Cartridge cart(rom, MapMode::LoRom);
    DummyScheduler sched;
    NullIo io;
    Bus<DummyScheduler, NullIo> bus(sched, cart, io);

    SECTION("Espejo: $7E:0000, $00:0000 y $80:0000") {
        bus.write8(0x7E0000, 0x12);
        REQUIRE(bus.read8(0x000000) == 0x12);
        REQUIRE(bus.read8(0x800000) == 0x12);
    }

    SECTION("Espejo: $7E:1FFF y $00:1FFF") {
        bus.write8(0x7E1FFF, 0x34);
        REQUIRE(bus.read8(0x001FFF) == 0x34);
    }

    SECTION("$00:2000 no es WRAM, devuelve open bus") {
        bus.write8(0x7E2000, 0x78); // guarda $78 en WRAM, índice 0x2000
        bus.write8(0x002001, 0x56); // deja $56 en el bus (dirección sin mapear)
        REQUIRE(bus.read8(0x002000) == 0x56); // open bus, no el $78 de la WRAM
    }

    SECTION("$7F:FFFF es el último byte") {
        bus.write8(0x7FFFFF, 0x9A);
        REQUIRE(bus.wram_state().data[0x1FFFF] == 0x9A);
        REQUIRE(bus.read8(0x7FFFFF) == 0x9A);
    }

    SECTION("Puerto $2180-$2183: lectura y escritura") {
        bus.write8(0x2181, 0xFF);
        bus.write8(0x2182, 0xFF);
        bus.write8(0x2183, 0x01); // WMADD = $01FFFF
        
        bus.write8(0x2180, 0xBB);
        
        // Verifica WRAM en $7F:FFFF
        REQUIRE(bus.read8(0x7FFFFF) == 0xBB);
        
        // La dirección del puerto debe haber vuelto a 0
        bus.write8(0x2180, 0xCC);
        REQUIRE(bus.read8(0x7E0000) == 0xCC);
    }

    SECTION("Bit 0 de WMADDH ($2183) es el único que cuenta") {
        bus.write8(0x2181, 0x00);
        bus.write8(0x2182, 0x00);
        bus.write8(0x2183, 0xFF); // WMADD = $010000

        bus.write8(0x2180, 0xDD);
        REQUIRE(bus.read8(0x7F0000) == 0xDD);
    }

    SECTION("Leer $2181 devuelve open bus") {
        bus.write8(0x000000, 0xEE); // Deja EE en open_bus (lee 000000 y graba EE en wram y open_bus)
        REQUIRE(bus.read8(0x2181) == 0xEE);
    }

    SECTION("Ida y vuelta de WramState") {
        bus.write8(0x7E0005, 0x11);
        bus.write8(0x2181, 0x05); // WMADD = 5
        
        WramState s = bus.wram_state();
        
        Bus<DummyScheduler, NullIo> bus2(sched, cart, io);
        bus2.load_wram_state(s);
        
        REQUIRE(bus2.read8(0x7E0005) == 0x11);
        REQUIRE(bus2.read8(0x2180) == 0x11);
    }
}
