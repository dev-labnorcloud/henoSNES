// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "serializer.h"
#include <cstring>

namespace heno::core {

Serializer::Serializer(SerializerMode mode, std::span<uint8_t> buffer)
    : m_mode(mode)
    , m_buffer(buffer)
    , m_offset(0)
    , m_current_version(0)
    , m_error(HENO_OK)
{
}

void Serializer::ensure_capacity(size_t size) {
    if (m_error != HENO_OK) return;
    if (m_offset + size > m_buffer.size()) {
        m_error = (m_mode == SerializerMode::Load) ? HENO_ERROR_DESERIALIZE : HENO_ERROR_SERIALIZE;
    }
}

void Serializer::section(const char* name, uint32_t version) {
    if (m_error != HENO_OK) return;
    
    // Simplistic section version tracking
    if (m_mode == SerializerMode::Save) {
        value32(version);
        m_current_version = version;
    } else {
        uint32_t loaded_version = 0;
        value32(loaded_version);
        if (m_error == HENO_OK) {
            m_current_version = loaded_version;
        }
    }
}

uint32_t Serializer::get_current_section_version() const {
    return m_current_version;
}

void Serializer::value8(uint8_t& val) {
    array(&val, sizeof(val));
}

void Serializer::value16(uint16_t& val) {
    array(reinterpret_cast<uint8_t*>(&val), sizeof(val));
}

void Serializer::value32(uint32_t& val) {
    array(reinterpret_cast<uint8_t*>(&val), sizeof(val));
}

void Serializer::value64(uint64_t& val) {
    array(reinterpret_cast<uint8_t*>(&val), sizeof(val));
}

void Serializer::array(uint8_t* data, size_t size) {
    if (size == 0) return;
    ensure_capacity(size);
    if (m_error != HENO_OK) return;

    if (m_mode == SerializerMode::Save) {
        std::memcpy(m_buffer.data() + m_offset, data, size);
    } else {
        std::memcpy(data, m_buffer.data() + m_offset, size);
    }
    m_offset += size;
}

} // namespace heno::core
