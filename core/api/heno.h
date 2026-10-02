// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

/// @file heno.h
/// @brief henoSNES public C API — stable ABI for frontends and tools.
///
/// This header defines the complete public interface to libheno-core.
/// All functions, structs, and enums are C-compatible (extern "C").
/// No C++ exceptions cross this boundary; errors are returned as HenoError codes.

#ifndef HENO_H
#define HENO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
// Version
// ---------------------------------------------------------------------------

#define HENO_API_VERSION_MAJOR 0
#define HENO_API_VERSION_MINOR 1
#define HENO_API_VERSION_PATCH 0

/// Returns a human-readable version string (e.g. "0.0.1-abc123").
const char* heno_version(void);

/// Returns the API version as a packed uint32: (major << 16) | (minor << 8) | patch.
uint32_t heno_api_version(void);

// ---------------------------------------------------------------------------
// Error codes
// ---------------------------------------------------------------------------

typedef enum HenoError {
    HENO_OK                     = 0,
    HENO_ERROR_NOT_INITIALIZED  = 1,
    HENO_ERROR_ALREADY_INIT     = 2,
    HENO_ERROR_INVALID_PARAM    = 3,
    HENO_ERROR_INVALID_ROM      = 4,
    HENO_ERROR_ROM_NOT_LOADED   = 5,
    HENO_ERROR_IO               = 6,
    HENO_ERROR_OUT_OF_MEMORY    = 7,
    HENO_ERROR_SERIALIZE        = 8,
    HENO_ERROR_DESERIALIZE      = 9,
    HENO_ERROR_NOT_IMPLEMENTED  = 10,
    HENO_ERROR_UNKNOWN          = 255
} HenoError;

/// Returns a human-readable string for the given error code.
const char* heno_error_string(HenoError err);

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

typedef enum HenoRegion {
    HENO_REGION_AUTO = 0,
    HENO_REGION_NTSC = 1,
    HENO_REGION_PAL  = 2
} HenoRegion;

typedef enum HenoPixelFormat {
    HENO_PIXEL_BGR555  = 0,   ///< 15-bit BGR, native SNES format
    HENO_PIXEL_XRGB8888 = 1   ///< 32-bit XRGB for convenience
} HenoPixelFormat;

typedef enum HenoLogLevel {
    HENO_LOG_TRACE   = 0,
    HENO_LOG_DEBUG   = 1,
    HENO_LOG_INFO    = 2,
    HENO_LOG_WARNING = 3,
    HENO_LOG_ERROR   = 4,
    HENO_LOG_FATAL   = 5
} HenoLogLevel;

typedef enum HenoMemoryDomain {
    HENO_MEM_WRAM   = 0,
    HENO_MEM_VRAM   = 1,
    HENO_MEM_OAM    = 2,
    HENO_MEM_CGRAM  = 3,
    HENO_MEM_SRAM   = 4,
    HENO_MEM_ROM    = 5,
    HENO_MEM_APURAM = 6
} HenoMemoryDomain;

// ---------------------------------------------------------------------------
// Structs
// ---------------------------------------------------------------------------

/// Video frame produced by the emulation core.
typedef struct HenoVideoFrame {
    const uint8_t* data;       ///< Pixel data (format depends on pixel_format)
    uint32_t       width;      ///< Frame width in pixels (256 or 512)
    uint32_t       height;     ///< Frame height in pixels (224, 239, 448, or 478)
    uint32_t       pitch;      ///< Bytes per row
    HenoPixelFormat pixel_format;
    uint8_t        interlace;  ///< Non-zero if interlaced frame
    uint8_t        overscan;   ///< Non-zero if overscan enabled
    uint8_t        _pad[2];    ///< Reserved for alignment
} HenoVideoFrame;

/// Audio samples produced by the emulation core.
typedef struct HenoAudioSamples {
    const int16_t* data;        ///< Interleaved stereo PCM samples
    uint32_t       frame_count; ///< Number of stereo frames (2 samples per frame)
    uint32_t       sample_rate; ///< Sample rate in Hz (32040 for SNES)
} HenoAudioSamples;

/// System information for the loaded ROM.
typedef struct HenoSystemInfo {
    char         title[22];     ///< Internal ROM title (null-terminated)
    HenoRegion   region;        ///< Detected or forced region
    uint32_t     rom_size;      ///< ROM size in bytes
    uint32_t     sram_size;     ///< SRAM size in bytes
    uint8_t      mapper;        ///< Mapper type (LoROM=0, HiROM=1, ExHiROM=2, etc.)
    uint8_t      has_battery;   ///< Non-zero if cartridge has battery-backed SRAM
    uint8_t      _pad[2];       ///< Reserved
} HenoSystemInfo;

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

/// Callback function types for frontend integration.
typedef void (*HenoVideoRefreshFn)(const HenoVideoFrame* frame, void* user_data);
typedef void (*HenoAudioSampleFn)(const HenoAudioSamples* samples, void* user_data);
typedef void (*HenoInputPollFn)(void* user_data);
typedef int16_t (*HenoInputStateFn)(uint8_t port, uint8_t device, uint16_t button, void* user_data);
typedef void (*HenoLogFn)(HenoLogLevel level, const char* msg, void* user_data);

typedef struct HenoCallbacks {
    HenoVideoRefreshFn video_refresh;
    HenoAudioSampleFn  audio_sample;
    HenoInputPollFn    input_poll;
    HenoInputStateFn   input_state;
    HenoLogFn          log;
    void*              user_data;     ///< Passed to all callbacks
} HenoCallbacks;

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

HenoError heno_init(void);
HenoError heno_deinit(void);

// ---------------------------------------------------------------------------
// ROM management
// ---------------------------------------------------------------------------

HenoError heno_load_rom(const uint8_t* data, size_t size);
HenoError heno_unload_rom(void);

// ---------------------------------------------------------------------------
// Emulation
// ---------------------------------------------------------------------------

HenoError heno_run_frame(void);
HenoError heno_reset(int hard);

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------

HenoError heno_set_callbacks(const HenoCallbacks* callbacks);

// ---------------------------------------------------------------------------
// State serialization
// ---------------------------------------------------------------------------

HenoError heno_serialize_size(size_t* out_size);
HenoError heno_serialize(uint8_t* buf, size_t size);
HenoError heno_deserialize(const uint8_t* buf, size_t size);

// ---------------------------------------------------------------------------
// Memory access (for cheats and debugger)
// ---------------------------------------------------------------------------

HenoError heno_read_memory(HenoMemoryDomain domain, uint32_t addr, uint8_t* buf, size_t size);
HenoError heno_write_memory(HenoMemoryDomain domain, uint32_t addr, uint8_t val);

// ---------------------------------------------------------------------------
// System info
// ---------------------------------------------------------------------------

HenoError heno_get_system_info(HenoSystemInfo* info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // HENO_H
