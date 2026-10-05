// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <type_traits>

namespace heno {

// Bits del registro de estado P.
inline constexpr std::uint8_t kFlagC = 0x01;
inline constexpr std::uint8_t kFlagZ = 0x02;
inline constexpr std::uint8_t kFlagI = 0x04;
inline constexpr std::uint8_t kFlagD = 0x08;
inline constexpr std::uint8_t kFlagX = 0x10;
inline constexpr std::uint8_t kFlagM = 0x20;
inline constexpr std::uint8_t kFlagV = 0x40;
inline constexpr std::uint8_t kFlagN = 0x80;

// Estado serializable de la CPU 65C816 (ADR-0004 §4, ADR-0005 regla 5).
struct CpuState {
    std::uint16_t a;   // acumulador C (B:A)
    std::uint16_t x;   // índice X
    std::uint16_t y;   // índice Y
    std::uint16_t s;   // puntero de pila
    std::uint16_t d;   // registro de página directa
    std::uint16_t pc;  // contador de programa (16 bits, dentro del banco pbr)
    std::uint8_t dbr;  // banco de datos
    std::uint8_t pbr;  // banco de programa
    std::uint8_t p;    // registro de estado (bits kFlag*)
    bool e;            // modo emulación
    bool waiting;      // WAI en curso (ADR-0004 §5)
    bool stopped;      // STP en curso (ADR-0004 §5)
};
static_assert(std::is_trivially_copyable_v<CpuState>);

} // namespace heno
