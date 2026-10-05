// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "cpu/cpu.h"
#include <cstdint>
#include <map>
#include <vector>

namespace heno::test {

namespace {

struct MockBus {
    std::map<std::uint32_t, std::uint8_t> memory;
    std::vector<std::uint32_t> reads;
    int idles = 0;

    std::uint8_t read8(std::uint32_t addr) {
        reads.push_back(addr);
        if (auto it = memory.find(addr); it != memory.end()) {
            return it->second;
        }
        return 0xEA; /* NOP */
    }
    void write8(std::uint32_t addr, std::uint8_t value) {}
    void idle() {
        idles++;
    }
    
    void clear_logs() {
        reads.clear();
        idles = 0;
    }
};

} // namespace

TEST_CASE("Addressing next_addr", "[cpu][addressing]") {
    CHECK(next_addr(0x12FFFF, Wrap::Linear) == 0x130000);
    CHECK(next_addr(0x12FFFF, Wrap::Bank) == 0x120000);
    CHECK(next_addr(0x0001FF, Wrap::Page) == 0x000100);
}

TEST_CASE("Addressing Modes", "[cpu][addressing]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    auto setup = [&](std::uint8_t opcode, std::uint8_t p = 0, bool e = false) {
        bus.memory.clear();
        bus.clear_logs();
        bus.memory[0x008000] = opcode;
        CpuState s{};
        s.pc = 0x8000;
        s.pbr = 0x00;
        s.p = p;
        s.e = e;
        if (e) {
            s.p = static_cast<std::uint8_t>(s.p | kFlagM | kFlagX);
            s.s = 0x01FF;
        }
        cpu.load_state(s);
    };

    SECTION("Immediate LDA # M8") {
        setup(0xA9, kFlagM | kFlagX);
        bus.memory[0x008001] = 0x42;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Value);
        CHECK(op.value == 0x42);
        CHECK(op.raw == 0x42);
    }
    
    SECTION("Immediate LDA # M16") {
        setup(0xA9, kFlagX);
        bus.memory[0x008001] = 0x34;
        bus.memory[0x008002] = 0x12;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Value);
        CHECK(op.value == 0x1234);
        CHECK(op.raw == 0x1234);
    }
    
    SECTION("Immediate LDX # X8") {
        setup(0xA2, kFlagM | kFlagX);
        bus.memory[0x008001] = 0x42;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Value);
        CHECK(op.value == 0x42);
    }
    
    SECTION("Immediate LDX # X16") {
        setup(0xA2, kFlagM);
        bus.memory[0x008001] = 0x34;
        bus.memory[0x008002] = 0x12;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Value);
        CHECK(op.value == 0x1234);
    }
    
    SECTION("Immediate REP") {
        setup(0xC2, 0); // REP is always 1 byte
        bus.memory[0x008001] = 0x42;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Value);
        CHECK(op.value == 0x42);
    }
    
    SECTION("Absolute") {
        setup(0xAD); // LDA Absolute
        bus.memory[0x008001] = 0x56;
        bus.memory[0x008002] = 0x34;
        CpuState s = cpu.state();
        s.dbr = 0x12;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Address);
        CHECK(op.addr == 0x123456);
        CHECK(op.wrap == Wrap::Linear);
    }
    
    SECTION("AbsoluteX") {
        setup(0xBD); // LDA Absolute,X
        bus.memory[0x008001] = 0xFF;
        bus.memory[0x008002] = 0xFF;
        CpuState s = cpu.state();
        s.dbr = 0x12;
        s.x = 2;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x130001); // Cruza banco
        CHECK(op.wrap == Wrap::Linear);
    }
    
    SECTION("AbsoluteLongX") {
        setup(0xBF); // LDA AbsoluteLong,X
        bus.memory[0x008001] = 0xFF;
        bus.memory[0x008002] = 0xFF;
        bus.memory[0x008003] = 0xFF;
        CpuState s = cpu.state();
        s.x = 1;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x000000); // 24-bit wrap
    }

    SECTION("Direct Native (no page wrap)") {
        setup(0xA5, 0, false); // LDA Direct
        bus.memory[0x008001] = 0x10;
        CpuState s = cpu.state();
        s.d = 0x1234;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x001244);
        CHECK(op.wrap == Wrap::Bank);
        
        setup(0xA5, 0, false);
        bus.memory[0x008001] = 0x20;
        s = cpu.state();
        s.d = 0xFFF0;
        cpu.load_state(s);
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.addr == 0x000010); // Confinado a banco 0
        CHECK(op.wrap == Wrap::Bank);
    }
    
    SECTION("DirectX Emulation with DL=0 (page wrap)") {
        setup(0xB5, kFlagM | kFlagX, true); // LDA Direct,X
        bus.memory[0x008001] = 0xF0;
        CpuState s = cpu.state();
        s.d = 0x0100;
        s.x = 0x20;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x000110);
        CHECK(op.wrap == Wrap::Page);
    }
    
    SECTION("DirectX Emulation with DL!=0 (no wrap)") {
        setup(0xB5, kFlagM | kFlagX, true);
        bus.memory[0x008001] = 0xF0;
        CpuState s = cpu.state();
        s.d = 0x0101;
        s.x = 0x20;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x000211);
        CHECK(op.wrap == Wrap::Bank);
    }
    
    SECTION("DirectX Native (no wrap)") {
        setup(0xB5, 0, false);
        bus.memory[0x008001] = 0xF0;
        CpuState s = cpu.state();
        s.d = 0x0100;
        s.x = 0x20;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x000210);
    }
    
    SECTION("DirectIndirect dp=$FF, D=0 (emulation vs native)") {
        setup(0xB2, kFlagM | kFlagX, true); // LDA (d)
        bus.memory[0x008001] = 0xFF;
        CpuState s = cpu.state();
        s.d = 0x0000;
        cpu.load_state(s);
        cpu.step();
        // ptr at $0000FF, next byte at $000000 (page wrap)
        REQUIRE(bus.reads.size() >= 4);
        CHECK(bus.reads[2] == 0x0000FF);
        CHECK(bus.reads[3] == 0x000000);
        
        setup(0xB2, 0, false); // native
        bus.memory[0x008001] = 0xFF;
        s = cpu.state();
        s.d = 0x0000;
        cpu.load_state(s);
        cpu.step();
        REQUIRE(bus.reads.size() >= 4);
        CHECK(bus.reads[2] == 0x0000FF);
        CHECK(bus.reads[3] == 0x000100);
    }
    
    SECTION("DirectIndexedIndirect emulation DL=0") {
        setup(0xA1, kFlagM | kFlagX, true); // LDA (d,x)
        bus.memory[0x008001] = 0xFE;
        CpuState s = cpu.state();
        s.d = 0x0100;
        s.x = 0x01;
        cpu.load_state(s);
        cpu.step();
        REQUIRE(bus.reads.size() >= 4);
        CHECK(bus.reads[2] == 0x0001FF);
        CHECK(bus.reads[3] == 0x000100); // Page wrap reading pointer
    }
    
    SECTION("DirectIndirectLong emulation D=0 dp=$FF") {
        setup(0xA7, kFlagM | kFlagX, true); // LDA [d]
        bus.memory[0x008001] = 0xFF;
        CpuState s = cpu.state();
        s.d = 0x0000;
        cpu.load_state(s);
        cpu.step();
        // No envuelve por página
        REQUIRE(bus.reads.size() >= 5);
        CHECK(bus.reads[2] == 0x0000FF);
        CHECK(bus.reads[3] == 0x000100);
        CHECK(bus.reads[4] == 0x000101);
    }
    
    SECTION("DirectIndirectIndexed") {
        setup(0xB1, 0, false); // LDA (d),y
        bus.memory[0x008001] = 0x00;
        bus.memory[0x000000] = 0xFF;
        bus.memory[0x000001] = 0xFF;
        CpuState s = cpu.state();
        s.d = 0x0000;
        s.dbr = 0x7E;
        s.y = 1;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x7F0000);
    }
    
    SECTION("StackRelative emulation") {
        setup(0xA3, kFlagM | kFlagX, true); // LDA d,S
        bus.memory[0x008001] = 0x20;
        CpuState s = cpu.state();
        s.s = 0x01F0;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x000210); // bank wrap, not page
    }
    
    SECTION("StackRelativeIndirectIndexed") {
        setup(0xB3, 0, false); // LDA (d,S),y
        bus.memory[0x008001] = 0x10;
        bus.memory[0x000110] = 0x00;
        bus.memory[0x000111] = 0x20;
        CpuState s = cpu.state();
        s.s = 0x0100;
        s.y = 0x10;
        s.dbr = 0x55;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x552010);
    }
    
    SECTION("AbsoluteIndirect") {
        setup(0x6C); // JMP (a)
        bus.memory[0x448000] = 0x6C;
        bus.memory[0x448001] = 0xFF;
        bus.memory[0x448002] = 0xFF;
        bus.memory[0x00FFFF] = 0x12;
        bus.memory[0x000000] = 0x34; // Bank wrap
        CpuState s = cpu.state();
        s.pbr = 0x44;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x443412);
        REQUIRE(bus.reads.size() >= 5);
        CHECK(bus.reads[3] == 0x00FFFF);
        CHECK(bus.reads[4] == 0x000000);
        
        setup(0xDC); // JML [a]
        bus.memory[0x008000] = 0xDC;
        bus.memory[0x008001] = 0xFF;
        bus.memory[0x008002] = 0xFF;
        bus.memory[0x00FFFF] = 0x12;
        bus.memory[0x000000] = 0x34;
        bus.memory[0x000001] = 0x56;
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.addr == 0x563412);
    }
    
    SECTION("AbsoluteIndexedIndirect") {
        setup(0x7C); // JMP (a,x)
        bus.memory[0x058000] = 0x7C;
        bus.memory[0x058001] = 0xFE;
        bus.memory[0x058002] = 0xFF;
        bus.memory[0x05FFFF] = 0x34;
        bus.memory[0x050000] = 0x12;
        CpuState s = cpu.state();
        s.pbr = 0x05;
        s.x = 1;
        cpu.load_state(s);
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x051234);
        REQUIRE(bus.reads.size() >= 5);
        CHECK(bus.reads[3] == 0x05FFFF);
        CHECK(bus.reads[4] == 0x050000);
    }
    
    SECTION("Relative") {
        setup(0x80); // BRA
        bus.memory[0x008001] = 0xFE;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x008000); // 8002 + (-2) = 8000
        
        setup(0x80);
        bus.memory[0x00FFFE] = 0x80;
        bus.memory[0x00FFFF] = 0x10;
        CpuState s = cpu.state();
        s.pc = 0xFFFE;
        s.pbr = 0x00;
        cpu.load_state(s);
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.addr == 0x000010); // bank wrap
    }
    
    SECTION("RelativeLong") {
        setup(0x82); // BRL
        bus.memory[0x008001] = 0xFF;
        bus.memory[0x008002] = 0xFF;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.addr == 0x008002); // 8003 + (-1) = 8002
    }
    
    SECTION("BlockMove") {
        setup(0x54); // MVN
        bus.memory[0x008001] = 0x7F; // dst
        bus.memory[0x008002] = 0x7E; // src
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::BlockMove);
        CHECK(op.dst_bank == 0x7F);
        CHECK(op.src_bank == 0x7E);
    }
    
    SECTION("Stack and Implied") {
        setup(0xF4); // PEA
        bus.memory[0x008001] = 0x34;
        bus.memory[0x008002] = 0x12;
        cpu.step();
        auto op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Raw);
        CHECK(op.raw == 0x1234);
        
        setup(0x00); // BRK
        bus.memory[0x008001] = 0x42;
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Raw);
        CHECK(op.raw == 0x42);
        
        setup(0x48); // PHA
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.kind == OperandKind::None);
        
        setup(0x42); // WDM
        bus.memory[0x008001] = 0xAA;
        cpu.step();
        op = cpu.last_operand();
        CHECK(op.kind == OperandKind::Raw);
        CHECK(op.raw == 0xAA);
    }
}

} // namespace heno::test
