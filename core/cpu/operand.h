// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <type_traits>

namespace heno {

enum class OperandKind : std::uint8_t { None, Value, Address, Raw, BlockMove };

enum class Wrap : std::uint8_t { Linear, Bank, Page };

constexpr std::uint32_t next_addr(std::uint32_t addr, Wrap w) noexcept {
    if (w == Wrap::Linear) {
        return (addr + 1u) & 0xFFFFFFu;
    }
    if (w == Wrap::Bank) {
        return (addr & 0xFF0000u) | ((addr + 1u) & 0xFFFFu);
    }
    return (addr & 0xFFFF00u) | ((addr + 1u) & 0xFFu);
}

struct Operand {
    OperandKind kind;
    Wrap wrap;
    std::uint32_t addr;
    std::uint16_t value;
    std::uint32_t raw;
    std::uint8_t dst_bank;
    std::uint8_t src_bank;
};

static_assert(std::is_trivially_copyable_v<Operand>);

} // namespace heno
