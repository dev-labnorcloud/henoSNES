// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <cstdint>
#include "core/sched/scheduler.h"
#include "core/cart/cart.h"

namespace heno::core {

class Serializer;

class Bus {
public:
    Bus(Scheduler& scheduler, Cartridge& cart);

    uint8_t read8(uint32_t addr24);
    void write8(uint32_t addr24, uint8_t value);
    
    void reset();

    void serialize(Serializer& s);

private:
    Scheduler& m_scheduler;
    Cartridge& m_cart;
    uint8_t m_wram[128 * 1024];
    
    uint8_t m_memsel;
    uint8_t m_open_bus;

    uint8_t read_internal(uint32_t addr24);
    void write_internal(uint32_t addr24, uint8_t value);
    uint32_t get_memory_cycles(uint32_t addr24) const;
    
    void debugger_hook(uint32_t addr24, uint8_t value, bool is_write);
};

} // namespace heno::core
