// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once
#include <cstdint>
#include <vector>
#include <span>

namespace heno::core {

class Cartridge {
public:
    Cartridge(std::span<const uint8_t> rom_data);

    uint8_t read(uint32_t addr24) const;
    void write(uint32_t addr24, uint8_t value);
    
    bool is_hirom() const { return m_is_hirom; }

private:
    std::vector<uint8_t> m_rom;
    bool m_is_hirom;
};

} // namespace heno::core
