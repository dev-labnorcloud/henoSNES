// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "cpu/opcode_table.h"
#include <map>
#include <string_view>

using namespace heno;

TEST_CASE("kOpcodeTable coverage and counts", "[cpu][opcode_table]") {
    REQUIRE(kOpcodeTable.size() == 256);

    std::map<AddrMode, int> counts;
    for (const auto& info : kOpcodeTable) {
        counts[info.mode]++;
    }

    CHECK(counts[AddrMode::Implied] == 29);
    CHECK(counts[AddrMode::Absolute] == 26);
    CHECK(counts[AddrMode::Direct] == 24);
    CHECK(counts[AddrMode::Stack] == 21);
    CHECK(counts[AddrMode::DirectX] == 18);
    CHECK(counts[AddrMode::AbsoluteX] == 17);
    CHECK(counts[AddrMode::Immediate] == 14);
    CHECK(counts[AddrMode::AbsoluteLong] == 10);
    CHECK(counts[AddrMode::Relative] == 9);
    CHECK(counts[AddrMode::AbsoluteY] == 9);
    CHECK(counts[AddrMode::StackRelative] == 8);
    CHECK(counts[AddrMode::AbsoluteLongX] == 8);
    CHECK(counts[AddrMode::DirectIndirectLongIndexed] == 8);
    CHECK(counts[AddrMode::DirectIndirectLong] == 8);
    CHECK(counts[AddrMode::DirectIndexedIndirect] == 8);
    CHECK(counts[AddrMode::StackRelativeIndirectIndexed] == 8);
    CHECK(counts[AddrMode::DirectIndirectIndexed] == 8);
    CHECK(counts[AddrMode::DirectIndirect] == 8);
    CHECK(counts[AddrMode::Accumulator] == 6);
    CHECK(counts[AddrMode::BlockMove] == 2);
    CHECK(counts[AddrMode::DirectY] == 2);
    CHECK(counts[AddrMode::AbsoluteIndexedIndirect] == 2);
    CHECK(counts[AddrMode::AbsoluteIndirect] == 2);
    CHECK(counts[AddrMode::RelativeLong] == 1);
}

TEST_CASE("kOpcodeTable specific cases", "[cpu][opcode_table]") {
    auto check_op = [](uint8_t op, Mnemonic m, AddrMode mode, uint8_t len) {
        CHECK(kOpcodeTable[op].mnemonic == m);
        CHECK(kOpcodeTable[op].mode == mode);
        CHECK(kOpcodeTable[op].base_bytes == len);
    };

    check_op(0x00, Mnemonic::BRK, AddrMode::Stack, 2);
    check_op(0x42, Mnemonic::WDM, AddrMode::Implied, 2);
    check_op(0x44, Mnemonic::MVP, AddrMode::BlockMove, 3);
    check_op(0x54, Mnemonic::MVN, AddrMode::BlockMove, 3);
    check_op(0x62, Mnemonic::PER, AddrMode::Stack, 3);
    check_op(0xD4, Mnemonic::PEI, AddrMode::Stack, 2);
    check_op(0xF4, Mnemonic::PEA, AddrMode::Stack, 3);
    check_op(0xDC, Mnemonic::JML, AddrMode::AbsoluteIndirect, 3);
    check_op(0x5C, Mnemonic::JMP, AddrMode::AbsoluteLong, 4);
    check_op(0x82, Mnemonic::BRL, AddrMode::RelativeLong, 3);
    check_op(0xCB, Mnemonic::WAI, AddrMode::Implied, 1);
    check_op(0xDB, Mnemonic::STP, AddrMode::Implied, 1);

    CHECK(kOpcodeTable[0xA9].mnemonic == Mnemonic::LDA);
    CHECK(kOpcodeTable[0xA9].mode == AddrMode::Immediate);
    CHECK(kOpcodeTable[0xA9].imm == ImmSize::M);

    CHECK(kOpcodeTable[0xA2].mnemonic == Mnemonic::LDX);
    CHECK(kOpcodeTable[0xA2].mode == AddrMode::Immediate);
    CHECK(kOpcodeTable[0xA2].imm == ImmSize::X);

    CHECK(kOpcodeTable[0xC2].mnemonic == Mnemonic::REP);
    CHECK(kOpcodeTable[0xC2].mode == AddrMode::Immediate);
    CHECK(kOpcodeTable[0xC2].imm == ImmSize::Fixed8);

    CHECK(kOpcodeTable[0xE2].mnemonic == Mnemonic::SEP);
    CHECK(kOpcodeTable[0xE2].mode == AddrMode::Immediate);
    CHECK(kOpcodeTable[0xE2].imm == ImmSize::Fixed8);
}

TEST_CASE("instruction_length calculation", "[cpu][opcode_table]") {
    CHECK(instruction_length(0xA9, true, true) == 2);
    CHECK(instruction_length(0xA9, true, false) == 2);
    CHECK(instruction_length(0xA9, false, true) == 3);
    CHECK(instruction_length(0xA9, false, false) == 3);

    CHECK(instruction_length(0xA2, true, true) == 2);
    CHECK(instruction_length(0xA2, false, true) == 2);
    CHECK(instruction_length(0xA2, true, false) == 3);
    CHECK(instruction_length(0xA2, false, false) == 3);

    CHECK(instruction_length(0xC2, true, true) == 2);
    CHECK(instruction_length(0xC2, false, false) == 2);
}

TEST_CASE("mnemonic_name stringification", "[cpu][opcode_table]") {
    CHECK(std::string_view(mnemonic_name(Mnemonic::ADC)) == "ADC");
    CHECK(std::string_view(mnemonic_name(Mnemonic::XCE)) == "XCE");
}
