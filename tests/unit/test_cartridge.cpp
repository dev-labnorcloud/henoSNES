// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "cart/cartridge.h"

using namespace heno;

TEST_CASE("Cartridge LoROM and HiROM Mapping", "[cartridge]") {
    // ROM de 64 KiB sintética
    std::vector<std::uint8_t> rom_data(65536);
    for (std::size_t i = 0; i < rom_data.size(); ++i) {
        rom_data[i] = static_cast<std::uint8_t>(i % 256);
    }
    
    // Función auxiliar para verificar que se lee el byte esperado del offset original
    auto check_read = [&](const Cartridge& cart, std::uint32_t addr, std::uint32_t expected_offset) {
        std::uint8_t val = 0;
        bool mapped = cart.read(addr, val);
        REQUIRE(mapped);
        REQUIRE(val == static_cast<std::uint8_t>(expected_offset % 256));
    };

    auto check_unmapped = [&](const Cartridge& cart, std::uint32_t addr) {
        std::uint8_t val = 0xFF; // dummy
        bool mapped = cart.read(addr, val);
        REQUIRE(!mapped);
    };

    SECTION("LoROM") {
        Cartridge cart(rom_data, MapMode::LoRom);
        
        check_read(cart, 0x008000, 0);
        check_read(cart, 0x00FFFF, 0x7FFF);
        check_read(cart, 0x018000, 0x8000);
        check_read(cart, 0x808000, 0); // espejo
        check_read(cart, 0x00FFC0, 0x7FC0);
        
        check_unmapped(cart, 0x000000);
        check_unmapped(cart, 0x007FFF);
        check_unmapped(cart, 0x7E8000);
        
        // Espejo: ROM de 64 KiB en LoROM, $02:8000 -> offset 0
        // Banco 2 -> (2 << 15) = 0x10000, pero rom.size() es 0x10000, asi que modulo da 0.
        check_read(cart, 0x028000, 0);
    }
    
    SECTION("HiROM") {
        Cartridge cart(rom_data, MapMode::HiRom);
        
        check_read(cart, 0xC00000, 0);
        check_read(cart, 0xC0FFC0, 0xFFC0);
        check_read(cart, 0x008000, 0x8000);
        check_read(cart, 0x00FFC0, 0xFFC0);
        check_read(cart, 0x400000, 0);
        
        check_unmapped(cart, 0x000000);
        check_unmapped(cart, 0x7E0000);
    }
}
