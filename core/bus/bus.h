// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "bus/access_speed.h"
#include "cart/cartridge.h"

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace heno {

template <typename T>
concept ClockSink = requires(T& s, std::uint32_t cycles) {
    { s.advance(cycles) } -> std::same_as<void>;
};

struct BusState {
    std::uint8_t open_bus;
    bool fastrom;
};
static_assert(std::is_trivially_copyable_v<BusState>);

// WRAM, E/S y el comportamiento detallado del open bus llegan en P1-007; DMA en su propia tarea.
template <ClockSink Scheduler>
class Bus {
public:
    using AccessHook = void (*)(void* ctx, std::uint32_t addr, std::uint8_t value, bool is_write);

    Bus(Scheduler& sched, const Cartridge& cart) noexcept
        : sched_(sched), cart_(cart) {}

    std::uint8_t read8(std::uint32_t addr) noexcept {
        sched_.advance(access_cycles(addr, state_.fastrom));
        
        std::uint8_t value = 0;
        if (cart_.read(addr, value)) {
            state_.open_bus = value;
        }
        
        if (hook_) {
            hook_(hook_ctx_, addr, state_.open_bus, false);
        }
        
        return state_.open_bus;
    }

    void write8(std::uint32_t addr, std::uint8_t value) noexcept {
        sched_.advance(access_cycles(addr, state_.fastrom));
        state_.open_bus = value;
        
        std::uint8_t bank = static_cast<std::uint8_t>(addr >> 16);
        std::uint16_t offset = static_cast<std::uint16_t>(addr & 0xFFFF);
        
        if (offset == 0x420D && ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF))) {
            state_.fastrom = (value & 1) != 0;
        }
        
        if (hook_) {
            hook_(hook_ctx_, addr, value, true);
        }
    }

    void idle() noexcept {
        sched_.advance(kInternalCycle);
    }

    void set_hook(AccessHook hook, void* ctx) noexcept {
        hook_ = hook;
        hook_ctx_ = ctx;
    }

    const BusState& state() const noexcept {
        return state_;
    }

    void load_state(const BusState& new_state) noexcept {
        state_ = new_state;
    }

private:
    Scheduler& sched_;
    const Cartridge& cart_;
    BusState state_{0, false};
    AccessHook hook_{nullptr};
    void* hook_ctx_{nullptr};
};

} // namespace heno
