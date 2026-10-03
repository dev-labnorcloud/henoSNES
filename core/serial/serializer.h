// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <vector>
#include "core/api/heno.h"

namespace heno::core {

enum class SerializerMode {
    Save,
    Load
};

class Serializer {
public:
    Serializer(SerializerMode mode, std::span<uint8_t> buffer);

    SerializerMode mode() const { return m_mode; }
    
    bool has_error() const { return m_error != HENO_OK; }
    HenoError error() const { return m_error; }

    void section(const char* name, uint32_t version);
    uint32_t get_current_section_version() const;

    void value8(uint8_t& val);
    void value16(uint16_t& val);
    void value32(uint32_t& val);
    void value64(uint64_t& val);
    void array(uint8_t* data, size_t size);

private:
    void ensure_capacity(size_t size);

    SerializerMode m_mode;
    std::span<uint8_t> m_buffer;
    size_t m_offset;
    uint32_t m_current_version;
    HenoError m_error;
};

} // namespace heno::core
