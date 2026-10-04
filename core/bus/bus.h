// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "bus/access_speed.h"
#include "bus/io_device.h"
#include "bus/wram.h"
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

// La semántica de cada registro de E/S llega con la tarea de su componente; DMA y SRAM (P1-014) quedan pendientes.
template <ClockSink Scheduler, IoDevice Io>
class Bus {
public:
    using AccessHook = void (*)(void* ctx, std::uint32_t addr, std::uint8_t value, bool is_write);

    Bus(Scheduler& sched, const Cartridge& cart, Io& io) noexcept
        : sched_(sched), cart_(cart), io_(io) {}

    std::uint8_t read8(std::uint32_t addr) noexcept {
        sched_.advance(access_cycles(addr, state_.fastrom));
        
        std::uint8_t bank = static_cast<std::uint8_t>(addr >> 16);
        std::uint16_t offset = static_cast<std::uint16_t>(addr & 0xFFFF);
        
        bool handled = false;
        std::uint8_t value = state_.open_bus;

        if (bank == 0x7E || bank == 0x7F) {
            std::uint32_t wram_idx = ((static_cast<std::uint32_t>(bank) & 1) << 16) | offset;
            value = wram_state_.data[wram_idx];
            handled = true;
        } else if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
            if (offset <= 0x1FFF) {
                value = wram_state_.data[offset];
                handled = true;
            } else if (offset == 0x2180) {
                value = wram_state_.data[wram_state_.port_addr];
                wram_state_.port_addr = (wram_state_.port_addr + 1) & 0x1FFFF;
                handled = true;
            } else if (offset >= 0x2181 && offset <= 0x2183) {
                // open bus
            } else if ((offset >= 0x2100 && offset <= 0x217F) || 
                       (offset >= 0x2184 && offset <= 0x21FF) || 
                       (offset >= 0x4000 && offset <= 0x43FF && offset != 0x420D)) {
                if (io_.read(addr, state_.open_bus, value)) {
                    handled = true;
                }
            } else if (offset == 0x420D) {
                // open bus
            }
        }

        if (!handled) {
            if (cart_.read(addr, value)) {
                handled = true;
            }
        }

        if (handled) {
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
        
        if (bank == 0x7E || bank == 0x7F) {
            std::uint32_t wram_idx = ((static_cast<std::uint32_t>(bank) & 1) << 16) | offset;
            wram_state_.data[wram_idx] = value;
        } else if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
            if (offset <= 0x1FFF) {
                wram_state_.data[offset] = value;
            } else if (offset == 0x2180) {
                wram_state_.data[wram_state_.port_addr] = value;
                wram_state_.port_addr = (wram_state_.port_addr + 1) & 0x1FFFF;
            } else if (offset == 0x2181) {
                wram_state_.port_addr = (wram_state_.port_addr & 0x1FF00) | value;
            } else if (offset == 0x2182) {
                wram_state_.port_addr = (wram_state_.port_addr & 0x100FF) | (static_cast<std::uint32_t>(value) << 8);
            } else if (offset == 0x2183) {
                wram_state_.port_addr = (wram_state_.port_addr & 0x0FFFF) | ((static_cast<std::uint32_t>(value) & 1) << 16);
            } else if ((offset >= 0x2100 && offset <= 0x217F) || 
                       (offset >= 0x2184 && offset <= 0x21FF) || 
                       (offset >= 0x4000 && offset <= 0x43FF && offset != 0x420D)) {
                io_.write(addr, value);
            } else if (offset == 0x420D) {
                state_.fastrom = (value & 1) != 0;
            }
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

    const WramState& wram_state() const noexcept {
        return wram_state_;
    }

    void load_wram_state(const WramState& new_state) noexcept {
        wram_state_ = new_state;
    }

private:
    Scheduler& sched_;
    const Cartridge& cart_;
    Io& io_;
    BusState state_{0, false};
    WramState wram_state_{};
    AccessHook hook_{nullptr};
    void* hook_ctx_{nullptr};
};

} // namespace heno
