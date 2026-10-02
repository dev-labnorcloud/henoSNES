// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "heno.h"

const char* heno_version(void) {
    return "0.0.1 (unknown)";
}

uint32_t heno_api_version(void) {
    return (HENO_API_VERSION_MAJOR << 16) | (HENO_API_VERSION_MINOR << 8) | HENO_API_VERSION_PATCH;
}

const char* heno_error_string(HenoError err) {
    switch (err) {
        case HENO_OK: return "OK";
        case HENO_ERROR_NOT_INITIALIZED: return "Not initialized";
        case HENO_ERROR_ALREADY_INIT: return "Already initialized";
        case HENO_ERROR_INVALID_PARAM: return "Invalid parameter";
        case HENO_ERROR_INVALID_ROM: return "Invalid ROM";
        case HENO_ERROR_ROM_NOT_LOADED: return "ROM not loaded";
        case HENO_ERROR_IO: return "I/O Error";
        case HENO_ERROR_OUT_OF_MEMORY: return "Out of memory";
        case HENO_ERROR_SERIALIZE: return "Serialization error";
        case HENO_ERROR_DESERIALIZE: return "Deserialization error";
        case HENO_ERROR_NOT_IMPLEMENTED: return "Not implemented";
        case HENO_ERROR_UNKNOWN: return "Unknown error";
        default: return "Unknown error";
    }
}

HenoError heno_init(void) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_deinit(void) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_load_rom(const uint8_t* data, size_t size) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_unload_rom(void) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_run_frame(void) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_reset(int hard) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_set_callbacks(const HenoCallbacks* callbacks) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_serialize_size(size_t* out_size) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_serialize(uint8_t* buf, size_t size) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_deserialize(const uint8_t* buf, size_t size) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_read_memory(HenoMemoryDomain domain, uint32_t addr, uint8_t* buf, size_t size) { return HENO_ERROR_NOT_IMPLEMENTED; }
HenoError heno_write_memory(HenoMemoryDomain domain, uint32_t addr, uint8_t val) { return HENO_ERROR_NOT_IMPLEMENTED; }

HenoError heno_get_system_info(HenoSystemInfo* info) {
    if (!info) return HENO_ERROR_INVALID_PARAM;
    return HENO_ERROR_NOT_IMPLEMENTED;
}
