// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "bus.h"
#include "core/serial/serializer.h"
#include <cstring>

namespace heno::core {

Bus::Bus(Scheduler& scheduler, Cartridge& cart)
    : m_scheduler(scheduler)
    , m_cart(cart)
    , m_memsel(0)
    , m_open_bus(0)
{
    std::memset(m_wram, 0, sizeof(m_wram));
}

void Bus::reset() {
    m_memsel = 0;
    m_open_bus = 0;
}

void Bus::debugger_hook(uint32_t /*addr24*/, uint8_t /*value*/, bool /*is_write*/) {
}

uint32_t Bus::get_memory_cycles(uint32_t addr24) const {
    uint8_t bank = (addr24 >> 16) & 0xFF;
    uint16_t offset = addr24 & 0xFFFF;

    bool is_fastrom = (m_memsel & 0x01) != 0;

    if (bank == 0x7E || bank == 0x7F) {
        return 8;
    }

    if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
        if (offset <= 0x1FFF) {
            return 8; 
        }
        if (offset >= 0x2000 && offset <= 0x3FFF) {
            return 6; 
        }
        if (offset >= 0x4000 && offset <= 0x41FF) {
            return 12; 
        }
        if (offset >= 0x4200 && offset <= 0x43FF) {
            return 6; 
        }
        if (offset >= 0x8000) {
            if (bank >= 0x80 && is_fastrom) {
                return 6; 
            } else {
                return 8; 
            }
        }
    }

    if (bank >= 0x40 && bank <= 0x7D) {
        return 8; 
    }

    if (bank >= 0xC0) {
        if (is_fastrom) {
            return 6; 
        } else {
            return 8; 
        }
    }

    return 8; 
}

uint8_t Bus::read8(uint32_t addr24) {
    uint32_t cycles = get_memory_cycles(addr24);
    
    uint8_t value = read_internal(addr24);
    m_open_bus = value;
    
    debugger_hook(addr24, value, false);
    m_scheduler.advance(cycles);
    
    return value;
}

void Bus::write8(uint32_t addr24, uint8_t value) {
    uint32_t cycles = get_memory_cycles(addr24);
    
    m_open_bus = value;
    write_internal(addr24, value);
    
    debugger_hook(addr24, value, true);
    m_scheduler.advance(cycles);
}

uint8_t Bus::read_internal(uint32_t addr24) {
    uint8_t bank = (addr24 >> 16) & 0xFF;
    uint16_t offset = addr24 & 0xFFFF;

    if (bank == 0x7E || bank == 0x7F) {
        uint32_t wram_addr = ((bank & 1) << 16) | offset;
        return m_wram[wram_addr];
    }

    if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
        if (offset <= 0x1FFF) {
            return m_wram[offset]; 
        }
        
        if (offset >= 0x2100 && offset <= 0x43FF) {
            if (offset == 0x420D) {
                return (m_open_bus & 0xFE) | (m_memsel & 0x01); 
            }
            return m_open_bus; 
        }
        
        if (offset >= 0x8000) {
            return m_cart.read(addr24);
        }
    }
    
    if ((bank >= 0x40 && bank <= 0x7D) || bank >= 0xC0) {
        return m_cart.read(addr24);
    }

    return m_open_bus;
}

void Bus::write_internal(uint32_t addr24, uint8_t value) {
    uint8_t bank = (addr24 >> 16) & 0xFF;
    uint16_t offset = addr24 & 0xFFFF;

    if (bank == 0x7E || bank == 0x7F) {
        uint32_t wram_addr = ((bank & 1) << 16) | offset;
        m_wram[wram_addr] = value;
        return;
    }

    if ((bank >= 0x00 && bank <= 0x3F) || (bank >= 0x80 && bank <= 0xBF)) {
        if (offset <= 0x1FFF) {
            m_wram[offset] = value; 
            return;
        }
        
        if (offset >= 0x2100 && offset <= 0x43FF) {
            if (offset == 0x420D) {
                m_memsel = value & 0x01;
            }
            return;
        }
        
        if (offset >= 0x8000) {
            m_cart.write(addr24, value);
            return;
        }
    }
    
    if ((bank >= 0x40 && bank <= 0x7D) || bank >= 0xC0) {
        m_cart.write(addr24, value);
        return;
    }
}

void Bus::serialize(Serializer& s) {
    s.section("Bus", 1);
    s.value8(m_memsel);
    s.value8(m_open_bus);
    s.array(m_wram, sizeof(m_wram));
}

} // namespace heno::core
