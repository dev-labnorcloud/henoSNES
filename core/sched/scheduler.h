// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

namespace heno::core {

class Serializer;

class ISchedulable {
public:
    virtual ~ISchedulable() = default;

    /// Catch up to the target master cycle.
    /// @warning This method must NOT be reentrant. If the component accesses the bus
    /// during catch_up, it must not trigger another catch_up in the scheduler.
    virtual void catch_up(uint64_t target_master_cycle) = 0;
};

enum class ComponentID : uint8_t {
    CPU = 0,
    PPU = 1,
    APU = 2,
    Max = 3
};

class Scheduler {
public:
    Scheduler();

    void register_component(ComponentID id, ISchedulable* comp);

    void advance(uint32_t master_cycles_consumed);
    
    uint64_t current_cycle() const;
    
    void reset();

    void serialize(Serializer& s);

    // Only for testing reentrancy
    bool is_in_catch_up() const { return m_in_catch_up; }

private:
    uint64_t m_current_cycle;
    ISchedulable* m_components[static_cast<uint8_t>(ComponentID::Max)];
    bool m_in_catch_up;
};

} // namespace heno::core
