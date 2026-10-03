// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "core/serial/serializer.h"
#include <vector>

using namespace heno::core;

TEST_CASE("Serializer roundtrip", "[serial]") {
    std::vector<uint8_t> buffer(256);
    
    uint8_t v8 = 0xAA;
    uint16_t v16 = 0x1234;
    uint32_t v32 = 0xDEADBEEF;
    uint64_t v64 = 0x1122334455667788;
    
    // Save
    {
        Serializer s(SerializerMode::Save, buffer);
        s.section("test", 1);
        s.value8(v8);
        s.value16(v16);
        s.value32(v32);
        s.value64(v64);
        REQUIRE(s.has_error() == false);
    }
    
    // Load
    {
        uint8_t l8 = 0;
        uint16_t l16 = 0;
        uint32_t l32 = 0;
        uint64_t l64 = 0;
        
        Serializer s(SerializerMode::Load, buffer);
        s.section("test", 1);
        REQUIRE(s.get_current_section_version() == 1);
        
        s.value8(l8);
        s.value16(l16);
        s.value32(l32);
        s.value64(l64);
        
        REQUIRE(s.has_error() == false);
        REQUIRE(l8 == v8);
        REQUIRE(l16 == v16);
        REQUIRE(l32 == v32);
        REQUIRE(l64 == v64);
    }
}

TEST_CASE("Serializer migration", "[serial]") {
    std::vector<uint8_t> buffer(256);
    
    // Save version 1
    {
        Serializer s(SerializerMode::Save, buffer);
        s.section("component", 1);
        uint8_t old_val = 0x55;
        s.value8(old_val);
    }
    
    // Load as version 2
    {
        Serializer s(SerializerMode::Load, buffer);
        s.section("component", 2); // Requested version 2, but buffer has version 1
        
        REQUIRE(s.get_current_section_version() == 1);
        
        uint8_t val = 0;
        s.value8(val);
        REQUIRE(val == 0x55);
        
        uint8_t new_val = 0; // Migrated value
        if (s.get_current_section_version() >= 2) {
            s.value8(new_val);
        } else {
            new_val = 0x99; // Default for old version
        }
        
        REQUIRE(s.has_error() == false);
        REQUIRE(new_val == 0x99);
    }
}

TEST_CASE("Serializer out of bounds", "[serial]") {
    std::vector<uint8_t> buffer(4);
    
    Serializer s(SerializerMode::Load, buffer);
    uint64_t v64 = 0;
    s.value64(v64); // Need 8 bytes, but buffer is 4
    
    REQUIRE(s.has_error() == true);
    REQUIRE(s.error() == HENO_ERROR_DESERIALIZE);
}
