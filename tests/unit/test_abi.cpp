// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "heno.h"
#include <cstring>
#include <string>

// ---- ABI stability: struct sizes ----
// These sizes are part of the ABI contract. If any change, it means
// a breaking change that requires a major version bump.

TEST_CASE("HenoVideoFrame struct size is stable", "[abi]") {
    CHECK(sizeof(HenoVideoFrame) > 0);
    #if INTPTR_MAX == INT64_MAX
        CHECK(sizeof(HenoVideoFrame) == 32); // 64-bit
    #else
        CHECK(sizeof(HenoVideoFrame) == 24); // 32-bit
    #endif
}

TEST_CASE("HenoAudioSamples struct size is stable", "[abi]") {
    #if INTPTR_MAX == INT64_MAX
        CHECK(sizeof(HenoAudioSamples) == 16); // 64-bit
    #else
        CHECK(sizeof(HenoAudioSamples) == 12); // 32-bit
    #endif
}

TEST_CASE("HenoSystemInfo struct size is stable", "[abi]") {
    CHECK(sizeof(HenoSystemInfo) == 40);
}

TEST_CASE("HenoCallbacks struct size is stable", "[abi]") {
    #if INTPTR_MAX == INT64_MAX
        CHECK(sizeof(HenoCallbacks) == 48); // 64-bit
    #else
        CHECK(sizeof(HenoCallbacks) == 24); // 32-bit
    #endif
}

// ---- ABI stability: enum values ----

TEST_CASE("HenoError enum values are stable", "[abi]") {
    CHECK(HENO_OK == 0);
    CHECK(HENO_ERROR_NOT_INITIALIZED == 1);
    CHECK(HENO_ERROR_ALREADY_INIT == 2);
    CHECK(HENO_ERROR_INVALID_PARAM == 3);
    CHECK(HENO_ERROR_INVALID_ROM == 4);
    CHECK(HENO_ERROR_ROM_NOT_LOADED == 5);
    CHECK(HENO_ERROR_IO == 6);
    CHECK(HENO_ERROR_OUT_OF_MEMORY == 7);
    CHECK(HENO_ERROR_SERIALIZE == 8);
    CHECK(HENO_ERROR_DESERIALIZE == 9);
    CHECK(HENO_ERROR_NOT_IMPLEMENTED == 10);
    CHECK(HENO_ERROR_UNKNOWN == 255);
}

TEST_CASE("HenoRegion enum values are stable", "[abi]") {
    CHECK(HENO_REGION_AUTO == 0);
    CHECK(HENO_REGION_NTSC == 1);
    CHECK(HENO_REGION_PAL == 2);
}

TEST_CASE("HenoPixelFormat enum values are stable", "[abi]") {
    CHECK(HENO_PIXEL_BGR555 == 0);
    CHECK(HENO_PIXEL_XRGB8888 == 1);
}

TEST_CASE("HenoLogLevel enum values are stable", "[abi]") {
    CHECK(HENO_LOG_TRACE == 0);
    CHECK(HENO_LOG_DEBUG == 1);
    CHECK(HENO_LOG_INFO == 2);
    CHECK(HENO_LOG_WARNING == 3);
    CHECK(HENO_LOG_ERROR == 4);
    CHECK(HENO_LOG_FATAL == 5);
}

TEST_CASE("HenoMemoryDomain enum values are stable", "[abi]") {
    CHECK(HENO_MEM_WRAM == 0);
    CHECK(HENO_MEM_VRAM == 1);
    CHECK(HENO_MEM_OAM == 2);
    CHECK(HENO_MEM_CGRAM == 3);
    CHECK(HENO_MEM_SRAM == 4);
    CHECK(HENO_MEM_ROM == 5);
    CHECK(HENO_MEM_APURAM == 6);
}

// ---- Version function ----

TEST_CASE("heno_version returns non-null string", "[api]") {
    const char* ver = heno_version();
    REQUIRE(ver != nullptr);
    CHECK(std::strlen(ver) > 0);
}

TEST_CASE("heno_api_version returns valid packed version", "[api]") {
    uint32_t v = heno_api_version();
    uint32_t major = (v >> 16) & 0xFF;
    uint32_t minor = (v >> 8) & 0xFF;
    uint32_t patch = v & 0xFF;
    CHECK(major == HENO_API_VERSION_MAJOR);
    CHECK(minor == HENO_API_VERSION_MINOR);
    CHECK(patch == HENO_API_VERSION_PATCH);
}

// ---- Stub behavior ----

TEST_CASE("Unimplemented functions return HENO_ERROR_NOT_IMPLEMENTED", "[api]") {
    CHECK(heno_run_frame() == HENO_ERROR_NOT_IMPLEMENTED);
    CHECK(heno_load_rom(nullptr, 0) == HENO_ERROR_NOT_IMPLEMENTED);
    CHECK(heno_unload_rom() == HENO_ERROR_NOT_IMPLEMENTED);
}

TEST_CASE("heno_error_string returns valid strings", "[api]") {
    CHECK(std::string(heno_error_string(HENO_OK)) == "OK");
    CHECK(std::strlen(heno_error_string(HENO_ERROR_NOT_IMPLEMENTED)) > 0);
    CHECK(std::strlen(heno_error_string(HENO_ERROR_UNKNOWN)) > 0);
}
