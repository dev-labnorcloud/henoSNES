// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "cpu/opcode_table.h"
#include "cpu/operand.h"

namespace heno {

template <AddrMode Mode>
struct Resolve;

constexpr Wrap get_direct_wrap(bool e, std::uint16_t d) noexcept {
    return (e && (d & 0xFFu) == 0) ? Wrap::Page : Wrap::Bank;
}

template <>
struct Resolve<AddrMode::Implied> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op{};
        const std::uint8_t bytes_to_read = info.base_bytes - 1;
        if (bytes_to_read > 0) {
            std::uint32_t raw = 0;
            for (std::uint8_t i = 0; i < bytes_to_read; ++i) {
                raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << (i * 8));
            }
            op.raw = raw;
            op.kind = OperandKind::Raw;
        } else {
            op.kind = OperandKind::None;
        }
        return op;
    }
};

template <>
struct Resolve<AddrMode::Stack> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        return Resolve<AddrMode::Implied>::run<E, M8, X8>(cpu, info);
    }
};

template <>
struct Resolve<AddrMode::Accumulator> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& /*cpu*/, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        op.kind = OperandKind::None;
        return op;
    }
};

template <>
struct Resolve<AddrMode::Immediate> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op{};
        op.kind = OperandKind::Value;
        bool is_16 = (info.imm == ImmSize::M && !M8) || (info.imm == ImmSize::X && !X8);
        std::uint16_t val = cpu.fetch8();
        if (is_16) {
            val = static_cast<std::uint16_t>(val | (static_cast<std::uint16_t>(cpu.fetch8()) << 8));
        }
        op.value = val;
        op.raw = val;
        return op;
    }
};

template <>
struct Resolve<AddrMode::Absolute> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint32_t raw = cpu.fetch8();
        raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << 8);
        op.raw = raw;
        op.addr = (static_cast<std::uint32_t>(cpu.state().dbr) << 16) | raw;
        op.wrap = Wrap::Linear;
        op.kind = OperandKind::Address;
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteX> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::Absolute>::run<E, M8, X8>(cpu, info);
        op.addr = (op.addr + cpu.state().x) & 0xFFFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteY> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::Absolute>::run<E, M8, X8>(cpu, info);
        op.addr = (op.addr + cpu.state().y) & 0xFFFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteLong> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint32_t raw = cpu.fetch8();
        raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << 8);
        raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << 16);
        op.raw = raw;
        op.addr = raw;
        op.wrap = Wrap::Linear;
        op.kind = OperandKind::Address;
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteLongX> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::AbsoluteLong>::run<E, M8, X8>(cpu, info);
        op.addr = (op.addr + cpu.state().x) & 0xFFFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteIndirect> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op{};
        std::uint32_t raw = cpu.fetch8();
        raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << 8);
        op.raw = raw;
        op.kind = OperandKind::Address;
        op.wrap = Wrap::Bank;
        
        std::uint32_t ptr_low = raw;
        std::uint32_t ptr_high = next_addr(ptr_low, Wrap::Bank);
        std::uint32_t target_low = cpu.read8(ptr_low);
        std::uint32_t target_high = cpu.read8(ptr_high);
        
        if (info.mnemonic == Mnemonic::JML) {
            std::uint32_t ptr_bank = next_addr(ptr_high, Wrap::Bank);
            std::uint32_t target_bank = cpu.read8(ptr_bank);
            op.addr = target_low | (target_high << 8) | (target_bank << 16);
        } else {
            std::uint32_t target = target_low | (target_high << 8);
            op.addr = (static_cast<std::uint32_t>(cpu.state().pbr) << 16) | target;
        }
        return op;
    }
};

template <>
struct Resolve<AddrMode::AbsoluteIndexedIndirect> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint32_t raw = cpu.fetch8();
        raw |= (static_cast<std::uint32_t>(cpu.fetch8()) << 8);
        op.raw = raw;
        op.kind = OperandKind::Address;
        op.wrap = Wrap::Bank;
        
        // Puntero en banco de programa
        std::uint32_t ptr_low = (static_cast<std::uint32_t>(cpu.state().pbr) << 16) | ((raw + cpu.state().x) & 0xFFFFu);
        std::uint32_t ptr_high = next_addr(ptr_low, Wrap::Bank);
        
        std::uint32_t target_low = cpu.read8(ptr_low);
        std::uint32_t target_high = cpu.read8(ptr_high);
        std::uint32_t target = target_low | (target_high << 8);
        
        op.addr = (static_cast<std::uint32_t>(cpu.state().pbr) << 16) | target;
        return op;
    }
};

template <>
struct Resolve<AddrMode::Direct> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint8_t dp = cpu.fetch8();
        op.raw = dp;
        op.kind = OperandKind::Address;
        op.wrap = get_direct_wrap(E, cpu.state().d);
        op.addr = (cpu.state().d + dp) & 0xFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectX> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint8_t dp = cpu.fetch8();
        op.raw = dp;
        op.kind = OperandKind::Address;
        std::uint16_t d = cpu.state().d;
        op.wrap = get_direct_wrap(E, d);
        
        if constexpr (E) {
            if ((d & 0xFFu) == 0) {
                op.addr = (d & 0xFF00u) | ((dp + cpu.state().x) & 0xFFu);
                return op;
            }
        }
        op.addr = (d + dp + cpu.state().x) & 0xFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectY> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint8_t dp = cpu.fetch8();
        op.raw = dp;
        op.kind = OperandKind::Address;
        std::uint16_t d = cpu.state().d;
        op.wrap = get_direct_wrap(E, d);
        
        if constexpr (E) {
            if ((d & 0xFFu) == 0) {
                op.addr = (d & 0xFF00u) | ((dp + cpu.state().y) & 0xFFu);
                return op;
            }
        }
        op.addr = (d + dp + cpu.state().y) & 0xFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectIndirect> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::Direct>::run<E, M8, X8>(cpu, info);
        std::uint32_t ptr_low = op.addr;
        std::uint32_t ptr_high = next_addr(ptr_low, op.wrap);
        std::uint32_t target = cpu.read8(ptr_low) | (static_cast<std::uint32_t>(cpu.read8(ptr_high)) << 8);
        
        op.addr = (static_cast<std::uint32_t>(cpu.state().dbr) << 16) | target;
        op.wrap = Wrap::Linear;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectIndexedIndirect> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::DirectX>::run<E, M8, X8>(cpu, info);
        std::uint32_t ptr_low = op.addr;
        std::uint32_t ptr_high = next_addr(ptr_low, op.wrap);
        std::uint32_t target = cpu.read8(ptr_low) | (static_cast<std::uint32_t>(cpu.read8(ptr_high)) << 8);
        
        op.addr = (static_cast<std::uint32_t>(cpu.state().dbr) << 16) | target;
        op.wrap = Wrap::Linear;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectIndirectIndexed> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::Direct>::run<E, M8, X8>(cpu, info);
        std::uint32_t ptr_low = op.addr;
        std::uint32_t ptr_high = next_addr(ptr_low, op.wrap);
        std::uint32_t target = cpu.read8(ptr_low) | (static_cast<std::uint32_t>(cpu.read8(ptr_high)) << 8);
        
        op.addr = (((static_cast<std::uint32_t>(cpu.state().dbr) << 16) | target) + cpu.state().y) & 0xFFFFFFu;
        op.wrap = Wrap::Linear;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectIndirectLong> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::Direct>::run<E, M8, X8>(cpu, info);
        std::uint32_t ptr_low = op.addr;
        std::uint32_t ptr_high = next_addr(ptr_low, Wrap::Bank);
        std::uint32_t ptr_bank = next_addr(ptr_high, Wrap::Bank);
        
        std::uint32_t target = cpu.read8(ptr_low);
        target |= (static_cast<std::uint32_t>(cpu.read8(ptr_high)) << 8);
        target |= (static_cast<std::uint32_t>(cpu.read8(ptr_bank)) << 16);
        
        op.addr = target;
        op.wrap = Wrap::Linear;
        return op;
    }
};

template <>
struct Resolve<AddrMode::DirectIndirectLongIndexed> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::DirectIndirectLong>::run<E, M8, X8>(cpu, info);
        op.addr = (op.addr + cpu.state().y) & 0xFFFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::StackRelative> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint8_t raw = cpu.fetch8();
        op.raw = raw;
        op.kind = OperandKind::Address;
        op.wrap = Wrap::Bank;
        op.addr = (cpu.state().s + raw) & 0xFFFFu;
        return op;
    }
};

template <>
struct Resolve<AddrMode::StackRelativeIndirectIndexed> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& info) noexcept {
        Operand op = Resolve<AddrMode::StackRelative>::run<E, M8, X8>(cpu, info);
        std::uint32_t ptr_low = op.addr;
        std::uint32_t ptr_high = next_addr(ptr_low, Wrap::Bank);
        std::uint32_t target = cpu.read8(ptr_low) | (static_cast<std::uint32_t>(cpu.read8(ptr_high)) << 8);
        
        op.addr = (((static_cast<std::uint32_t>(cpu.state().dbr) << 16) | target) + cpu.state().y) & 0xFFFFFFu;
        op.wrap = Wrap::Linear;
        return op;
    }
};

template <>
struct Resolve<AddrMode::Relative> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint8_t raw = cpu.fetch8();
        op.raw = raw;
        op.kind = OperandKind::Address;
        op.wrap = Wrap::Bank;
        
        std::int8_t offset = static_cast<std::int8_t>(raw);
        std::uint16_t target = static_cast<std::uint16_t>(cpu.state().pc + offset);
        op.addr = (static_cast<std::uint32_t>(cpu.state().pbr) << 16) | target;
        return op;
    }
};

template <>
struct Resolve<AddrMode::RelativeLong> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        std::uint16_t raw = cpu.fetch8();
        raw |= (static_cast<std::uint16_t>(cpu.fetch8()) << 8);
        op.raw = raw;
        op.kind = OperandKind::Address;
        op.wrap = Wrap::Bank;
        
        std::int16_t offset = static_cast<std::int16_t>(raw);
        std::uint16_t target = static_cast<std::uint16_t>(cpu.state().pc + offset);
        op.addr = (static_cast<std::uint32_t>(cpu.state().pbr) << 16) | target;
        return op;
    }
};

template <>
struct Resolve<AddrMode::BlockMove> {
    template <bool E, bool M8, bool X8, class C>
    static Operand run(C& cpu, const OpcodeInfo& /*info*/) noexcept {
        Operand op{};
        op.kind = OperandKind::BlockMove;
        op.dst_bank = cpu.fetch8();
        op.src_bank = cpu.fetch8();
        op.raw = static_cast<std::uint16_t>(op.dst_bank) | (static_cast<std::uint16_t>(op.src_bank) << 8);
        return op;
    }
};

} // namespace heno
