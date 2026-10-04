// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <concepts>
#include <cstdint>

namespace heno {

template <typename T>
concept IoDevice = requires(T& d, std::uint32_t addr, std::uint8_t open_bus, std::uint8_t& out, std::uint8_t value) {
    { d.read(addr, open_bus, out) } -> std::same_as<bool>;
    { d.write(addr, value) } -> std::same_as<bool>;
};

struct NullIo {
    bool read(std::uint32_t /*addr*/, std::uint8_t /*open_bus*/, std::uint8_t& /*out*/) noexcept {
        return false;
    }
    bool write(std::uint32_t /*addr*/, std::uint8_t /*value*/) noexcept {
        return false;
    }
};
static_assert(IoDevice<NullIo>);

} // namespace heno
