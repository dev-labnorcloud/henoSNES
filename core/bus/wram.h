// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <array>
#include <cstdint>
#include <type_traits>

namespace heno {

struct WramState {
    std::array<std::uint8_t, 0x20000> data{};
    std::uint32_t port_addr = 0;
};

static_assert(std::is_trivially_copyable_v<WramState>);

} // namespace heno
