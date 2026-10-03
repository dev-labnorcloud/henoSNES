// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "scheduler.h"
#include "core/serial/serializer.h"

namespace heno::core {

Scheduler::Scheduler()
    : m_current_cycle(0)
    , m_in_catch_up(false)
{
    for (int i = 0; i < static_cast<int>(ComponentID::Max); ++i) {
        m_components[i] = nullptr;
    }
}

void Scheduler::register_component(ComponentID id, ISchedulable* comp) {
    if (static_cast<uint8_t>(id) < static_cast<uint8_t>(ComponentID::Max)) {
        m_components[static_cast<uint8_t>(id)] = comp;
    }
}

void Scheduler::advance(uint32_t master_cycles_consumed) {
    m_current_cycle += master_cycles_consumed;
    
    if (m_in_catch_up) {
        // Prevent reentrancy
        return;
    }
    
    m_in_catch_up = true;
    for (int i = 0; i < static_cast<int>(ComponentID::Max); ++i) {
        if (m_components[i]) {
            m_components[i]->catch_up(m_current_cycle);
        }
    }
    m_in_catch_up = false;
}

uint64_t Scheduler::current_cycle() const {
    return m_current_cycle;
}

void Scheduler::reset() {
    m_current_cycle = 0;
    m_in_catch_up = false;
}

void Scheduler::serialize(Serializer& s) {
    s.section("Scheduler", 1);
    s.value64(m_current_cycle);
}

} // namespace heno::core
