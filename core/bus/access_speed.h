// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

namespace heno {

inline constexpr std::uint8_t kInternalCycle = 6;

constexpr std::uint8_t access_cycles(std::uint32_t addr, bool fastrom) noexcept {
    std::uint8_t bank = static_cast<std::uint8_t>(addr >> 16);
    std::uint16_t offset = static_cast<std::uint16_t>(addr & 0xFFFF);

    if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
        if (offset <= 0x1FFF) {
            return 8;
        } else if (offset >= 0x2000 && offset <= 0x3FFF) {
            return 6;
        } else if (offset >= 0x4000 && offset <= 0x41FF) {
            return 12;
        } else if (offset >= 0x4200 && offset <= 0x5FFF) {
            return 6;
        } else if (offset >= 0x6000 && offset <= 0x7FFF) {
            return 8;
        } else { // 0x8000 - 0xFFFF
            if (bank >= 0x80 && fastrom) {
                return 6;
            }
            return 8;
        }
    } else if (bank >= 0x40 && bank <= 0x7F) {
        return 8;
    } else { // 0xC0 - 0xFF
        if (fastrom) {
            return 6;
        }
        return 8;
    }
}

} // namespace heno
