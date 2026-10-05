// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/assert.h"
#include "cpu/cpu_state.h"
#include "cpu/opcode_table.h"
#include "cpu/addressing.h"
#include "cpu/execute.h"

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace heno {

template <typename B>
concept CpuBus = requires(B& b, std::uint32_t addr, std::uint8_t value) {
    { b.read8(addr) } -> std::same_as<std::uint8_t>;
    { b.write8(addr, value) } -> std::same_as<void>;
    { b.idle() } -> std::same_as<void>;
};

// Última instrucción decodificada. Caché de depuración: no forma parte de CpuState.
struct DecodeTrace {
    std::uint32_t addr; // dirección de 24 bits del byte de opcode
    std::uint8_t opcode;
    Mnemonic mnemonic;
    AddrMode mode;
    std::uint8_t length;
};

// Intérprete de la CPU 65C816 con despacho por tablas de plantillas (ADR-0004 §2).
// P1-001a: solo decodificación y lectura de operandos. Los modos de direccionamiento
// (P1-001b), la semántica (P1-002), las interrupciones y el vector de RESET (P1-003)
// y los ciclos internos (P1-004) llegan en sus tareas.
template <CpuBus BusT>
class Cpu {
public:
    using Handler = void (*)(Cpu&) noexcept;
    using HandlerTable = std::array<Handler, 256>;

    // Estado inicial de modo emulación; el vector de RESET se implementa en P1-003.
    explicit Cpu(BusT& bus) noexcept : bus_(bus) {
        state_.p = static_cast<std::uint8_t>(kFlagM | kFlagX | kFlagI);
        state_.s = 0x01FF;
        state_.e = true;
        refresh_table();
    }

    // Ejecuta una instrucción. Con WAI o STP activos consume un ciclo interno y retorna
    // (ADR-0004 §5); despertar con interrupciones es P1-003.
    void step() noexcept {
        if (state_.waiting || state_.stopped) {
            bus_.idle();
            return;
        }
        const std::uint8_t opcode = fetch8();
        (*active_table_)[opcode](*this);
    }

    // Punto único de cambio de P (REP, SEP, PLP y RTI en P1-002).
    void set_p(std::uint8_t value) noexcept {
        if (state_.e) {
            value = static_cast<std::uint8_t>(value | kFlagM | kFlagX);
        }
        state_.p = value;
        if ((state_.p & kFlagX) != 0) {
            clear_index_high_bytes();
        }
        refresh_table();
    }

    // Cambio del flag E (XCE en P1-002). Al entrar en modo emulación aplica
    // docs/refs/65c816-quirks.md §1; al salir no cambia ningún registro.
    void set_emulation(bool e) noexcept {
        if (e && !state_.e) {
            state_.p = static_cast<std::uint8_t>(state_.p | kFlagM | kFlagX);
            clear_index_high_bytes();
            state_.s = static_cast<std::uint16_t>(0x0100u | (state_.s & 0x00FFu));
        }
        state_.e = e;
        refresh_table();
    }

    const CpuState& state() const noexcept { return state_; }

    void load_state(const CpuState& s) noexcept {
        if (s.e) {
            HENO_ASSERT((s.p & kFlagM) != 0);
            HENO_ASSERT((s.p & kFlagX) != 0);
            HENO_ASSERT((s.x & 0xFF00u) == 0);
            HENO_ASSERT((s.y & 0xFF00u) == 0);
            HENO_ASSERT((s.s & 0xFF00u) == 0x0100u);
        }
        state_ = s;
        refresh_table();
    }

    const DecodeTrace& last_decode() const noexcept { return last_decode_; }
    const Operand& last_operand() const noexcept { return last_operand_; }

private:
    template <AddrMode> friend struct Resolve;
    template <Mnemonic> friend struct Exec;

    // Lee un byte del bus.
    std::uint8_t read8(std::uint32_t addr) noexcept {
        return bus_.read8(addr);
    }
    // Un handler por opcode y modo. M8 / X8 = true: registro de 8 bits.
    template <std::uint8_t Op, bool E, bool M8, bool X8>
    static void handler(Cpu& cpu) noexcept {
        static_assert(!E || (M8 && X8), "en modo emulación M y X valen 1");
        constexpr OpcodeInfo kInfo = kOpcodeTable[Op];
        constexpr std::uint8_t kLength = instruction_length(Op, M8, X8);

        const auto opcode_pc = static_cast<std::uint16_t>(cpu.state_.pc - 1u);
        cpu.last_decode_ = DecodeTrace{
            (static_cast<std::uint32_t>(cpu.state_.pbr) << 16) | opcode_pc,
            Op,
            kInfo.mnemonic,
            kInfo.mode,
            kLength,
        };
        cpu.last_operand_ = Resolve<kInfo.mode>::template run<E, M8, X8>(cpu, kInfo);
        Exec<kInfo.mnemonic>::template run<E, M8, X8>(cpu, cpu.last_operand_);
    }

    template <bool E, bool M8, bool X8, std::size_t... I>
    static constexpr HandlerTable make_table(std::index_sequence<I...>) noexcept {
        return HandlerTable{{&handler<static_cast<std::uint8_t>(I), E, M8, X8>...}};
    }

    template <bool E, bool M8, bool X8>
    static constexpr HandlerTable kTable =
        make_table<E, M8, X8>(std::make_index_sequence<256>{});

    // Recalcula la tabla activa a partir de E, M y X. No se serializa (ADR-0004 §4).
    void refresh_table() noexcept {
        if (state_.e) {
            active_table_ = &kTable<true, true, true>;
            return;
        }
        const bool m8 = (state_.p & kFlagM) != 0;
        const bool x8 = (state_.p & kFlagX) != 0;
        if (m8) {
            active_table_ = x8 ? &kTable<false, true, true> : &kTable<false, true, false>;
        } else {
            active_table_ = x8 ? &kTable<false, false, true> : &kTable<false, false, false>;
        }
    }

    // Lee el byte en (pbr << 16) | pc y avanza pc.
    std::uint8_t fetch8() noexcept {
        const std::uint32_t addr = (static_cast<std::uint32_t>(state_.pbr) << 16) | state_.pc;
        const std::uint8_t value = bus_.read8(addr);
        // PC es de 16 bits y envuelve dentro del banco: la hoja de datos de WDC indica que el Program Bank Register no cambia al incrementar el PC desde $FFFF.
        state_.pc = static_cast<std::uint16_t>(state_.pc + 1u);
        return value;
    }

    void clear_index_high_bytes() noexcept {
        state_.x = static_cast<std::uint16_t>(state_.x & 0x00FFu);
        state_.y = static_cast<std::uint16_t>(state_.y & 0x00FFu);
    }

    BusT& bus_;
    CpuState state_{};
    const HandlerTable* active_table_{nullptr};
    DecodeTrace last_decode_{};
    Operand last_operand_{};
};

} // namespace heno
