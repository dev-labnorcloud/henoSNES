// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "cpu/cpu.h"
#include <cstdint>
#include <map>
#include <vector>

namespace heno::test {

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

TEST_CASE("Cpu decode all 256 opcodes in 5 combinations", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);

    struct Combo {
        bool e, m, x;
        std::uint8_t p;
    };

    const Combo combos[] = {
        { true,  true,  true,  0b00110000 },
        { false, true,  true,  0b00110000 },
        { false, true,  false, 0b00100000 },
        { false, false, true,  0b00010000 },
        { false, false, false, 0b00000000 }
    };

    for (const auto& combo : combos) {
        for (int op = 0; op < 256; ++op) {
            bus.memory.clear();
            bus.clear_logs();
            
            std::uint8_t opcode = static_cast<std::uint8_t>(op);
            CAPTURE(op, combo.e, combo.m, combo.x);
            bus.memory[0x008000] = opcode;
            for (int i = 1; i < 4; ++i) {
                bus.memory[0x008000 + i] = 0x00; // padding bytes
            }

            CpuState state{};
            state.pbr = 0x00;
            state.pc = 0x8000;
            state.p = combo.p;
            state.e = combo.e;
            if (combo.e) {
                state.s = 0x01FF;
            }
            cpu.load_state(state);

            cpu.step();

            auto trace = cpu.last_decode();
            CHECK(trace.opcode == opcode);
            CHECK(trace.mnemonic == kOpcodeTable[opcode].mnemonic);
            CHECK(trace.mode == kOpcodeTable[opcode].mode);
            
            std::uint8_t expected_len = instruction_length(opcode, combo.m, combo.x);
            CHECK(trace.length == expected_len);
            CHECK(cpu.state().pc == static_cast<std::uint16_t>(0x8000 + expected_len));

            CHECK(bus.reads.size() == expected_len);
            for (std::size_t i = 0; i < expected_len; ++i) {
                CHECK(bus.reads[i] == static_cast<std::uint32_t>(0x008000 + i));
            }
        }
    }
}

TEST_CASE("Cpu table selection", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    SECTION("LDA # length depends on M") {
        bus.memory[0x008000] = 0xA9;
        
        cpu.set_emulation(false);
        cpu.set_p(0x30); // M=1, X=1
        CpuState s = cpu.state();
        s.pc = 0x8000;
        s.pbr = 0x00;
        cpu.load_state(s);
        
        bus.clear_logs();
        cpu.step();
        CHECK(cpu.last_decode().length == 2);
        CHECK(cpu.state().pc == 0x8002);
        
        cpu.set_p(0x10); // M=0, X=1
        s = cpu.state();
        s.pc = 0x8000;
        cpu.load_state(s);
        bus.clear_logs();
        cpu.step();
        CHECK(cpu.last_decode().length == 3);
        CHECK(cpu.state().pc == 0x8003);
    }
    
    SECTION("LDX # length depends on X") {
        bus.memory[0x008000] = 0xA2;
        
        cpu.set_emulation(false);
        cpu.set_p(0x30); // M=1, X=1
        CpuState s = cpu.state();
        s.pc = 0x8000;
        s.pbr = 0x00;
        cpu.load_state(s);
        
        bus.clear_logs();
        cpu.step();
        CHECK(cpu.last_decode().length == 2);
        
        cpu.set_p(0x20); // M=1, X=0
        s = cpu.state();
        s.pc = 0x8000;
        cpu.load_state(s);
        bus.clear_logs();
        cpu.step();
        CHECK(cpu.last_decode().length == 3);
    }
    
    SECTION("set_emulation(true) effects") {
        CpuState s = cpu.state();
        s.x = 0x1234;
        s.y = 0x5678;
        s.s = 0x03FF;
        s.p = 0x00;
        s.e = false;
        cpu.load_state(s);
        
        cpu.set_emulation(true);
        CHECK(cpu.state().e == true);
        CHECK((cpu.state().p & kFlagM) != 0);
        CHECK((cpu.state().p & kFlagX) != 0);
        CHECK((cpu.state().x & 0xFF00) == 0);
        CHECK((cpu.state().y & 0xFF00) == 0);
        CHECK((cpu.state().s & 0xFF00) == 0x0100);
        CHECK((cpu.state().s & 0x00FF) == 0x00FF);
        
        bus.memory[0x008000] = 0xA9; // LDA #
        s = cpu.state();
        s.pc = 0x8000;
        s.pbr = 0x00;
        cpu.load_state(s);
        bus.clear_logs();
        cpu.step();
        CHECK(cpu.last_decode().length == 2);
    }
    
    SECTION("set_emulation(false) maintains M and X") {
        cpu.set_emulation(true);
        cpu.set_emulation(false);
        CHECK((cpu.state().p & kFlagM) != 0);
        CHECK((cpu.state().p & kFlagX) != 0);
    }
}

TEST_CASE("Cpu set_p index high bytes", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    cpu.set_emulation(false);
    CpuState s = cpu.state();
    s.x = 0x1234;
    s.y = 0x5678;
    cpu.load_state(s);
    
    cpu.set_p(0x00); // X=0
    CHECK(cpu.state().x == 0x1234);
    CHECK(cpu.state().y == 0x5678);
    
    cpu.set_p(0x10); // X=1
    CHECK(cpu.state().x == 0x0034);
    CHECK(cpu.state().y == 0x0078);
}

TEST_CASE("Cpu PC wrapping", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    bus.memory[0x00FFFF] = 0xA9; // LDA #
    bus.memory[0x000000] = 0x42; // Operando en 00:0000
    
    CpuState s = cpu.state();
    s.pbr = 0x00;
    s.pc = 0xFFFF;
    s.p = 0x30; // M=1, 2 bytes
    s.e = false;
    cpu.load_state(s);
    
    bus.clear_logs();
    cpu.step();
    
    CHECK(cpu.last_decode().addr == 0x00FFFF);
    CHECK(cpu.last_decode().length == 2);
    CHECK(cpu.state().pc == 0x0001);
    CHECK(cpu.state().pbr == 0x00);
    CHECK(bus.reads.size() == 2);
    CHECK(bus.reads[0] == 0x00FFFF);
    CHECK(bus.reads[1] == 0x000000);
}

TEST_CASE("Cpu idle states", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    SECTION("waiting") {
        CpuState s = cpu.state();
        s.waiting = true;
        cpu.load_state(s);
        
        bus.clear_logs();
        cpu.step();
        CHECK(bus.idles == 1);
        CHECK(bus.reads.size() == 0);
    }
    
    SECTION("stopped") {
        CpuState s = cpu.state();
        s.stopped = true;
        cpu.load_state(s);
        
        bus.clear_logs();
        cpu.step();
        CHECK(bus.idles == 1);
        CHECK(bus.reads.size() == 0);
    }
}

TEST_CASE("CpuState load/save roundtrip", "[cpu][decode]") {
    MockBus bus;
    Cpu<MockBus> cpu(bus);
    
    CpuState s{};
    s.a = 0x1122;
    s.x = 0x3344;
    s.y = 0x5566;
    s.s = 0x7788;
    s.d = 0x99AA;
    s.pc = 0xBBCC;
    s.pbr = 0xDD;
    s.dbr = 0xEE;
    s.p = 0x10; // X=1
    s.e = false;
    s.waiting = true;
    s.stopped = false;
    
    cpu.load_state(s);
    CpuState s2 = cpu.state();
    
    CHECK(s.a == s2.a);
    CHECK(s.x == s2.x);
    CHECK(s.y == s2.y);
    CHECK(s.s == s2.s);
    CHECK(s.d == s2.d);
    CHECK(s.pc == s2.pc);
    CHECK(s.pbr == s2.pbr);
    CHECK(s.dbr == s2.dbr);
    CHECK(s.p == s2.p);
    CHECK(s.e == s2.e);
    CHECK(s.waiting == s2.waiting);
    CHECK(s.stopped == s2.stopped);
}

} // namespace heno::test
