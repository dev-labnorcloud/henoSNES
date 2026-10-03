// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cart.h"

namespace heno::core {

Cartridge::Cartridge(std::span<const uint8_t> rom_data)
    : m_rom(rom_data.begin(), rom_data.end())
    , m_is_hirom(false)
{
    if (m_rom.size() >= 0x10000) {
        if (m_rom.size() >= 0xFFD6 && m_rom[0xFFD5] == 0x21) {
            m_is_hirom = true;
        }
    }
}

uint8_t Cartridge::read(uint32_t addr24) const {
    if (m_rom.empty()) return 0x00;

    uint32_t bank = (addr24 >> 16) & 0xFF;
    uint32_t offset = addr24 & 0xFFFF;

    uint32_t rom_addr = 0;

    if (m_is_hirom) {
        rom_addr = ((bank & 0x3F) << 16) | offset;
    } else {
        if (offset < 0x8000) return 0x00; 
        rom_addr = ((bank & 0x7F) << 15) | (offset & 0x7FFF);
    }

    if (rom_addr < m_rom.size()) {
        return m_rom[rom_addr];
    }
    return 0x00; 
}

void Cartridge::write(uint32_t /*addr24*/, uint8_t /*value*/) {
}

} // namespace heno::core
