// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/assert.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace heno {

enum class MapMode { LoRom, HiRom };

class Cartridge {
public:
    Cartridge(std::vector<std::uint8_t> rom, MapMode mode)
        : rom_(std::move(rom)), mode_(mode) {
        HENO_ASSERT(!rom_.empty());
    }

    bool read(std::uint32_t addr, std::uint8_t& out) const noexcept {
        std::uint8_t bank = static_cast<std::uint8_t>(addr >> 16);
        std::uint16_t offset = static_cast<std::uint16_t>(addr & 0xFFFF);

        // Bancos $7E-$7F nunca son cartucho.
        if (bank == 0x7E || bank == 0x7F) {
            return false;
        }

        std::uint32_t rom_offset = 0;

        if (mode_ == MapMode::LoRom) {
            // LoROM: mapea solo $8000-$FFFF de los bancos $00-$7D y $80-$FF.
            if (offset < 0x8000) {
                return false;
            }
            rom_offset = ((static_cast<std::uint32_t>(bank) & 0x7F) << 15) | (static_cast<std::uint32_t>(offset) & 0x7FFF);
        } else {
            // HiROM: bancos $C0-$FF y $40-$7D, $0000-$FFFF:
            // Bancos $00-$3F y $80-$BF, solo $8000-$FFFF:
            if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
                if (offset < 0x8000) {
                    return false;
                }
            }
            rom_offset = ((static_cast<std::uint32_t>(bank) & 0x3F) << 16) | static_cast<std::uint32_t>(offset);
        }

        // Espejo: offset % rom.size(). La réplica exacta para tamaños que no son potencia de 2 queda [verificar].
        out = rom_[rom_offset % rom_.size()];
        return true;
    }

private:
    std::vector<std::uint8_t> rom_;
    MapMode mode_;
};

} // namespace heno
