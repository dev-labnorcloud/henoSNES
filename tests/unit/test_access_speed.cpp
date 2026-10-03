// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <catch2/catch_test_macros.hpp>
#include "bus/access_speed.h"

using namespace heno;

TEST_CASE("Access Speed Cycles", "[bus]") {
    SECTION("Bank 00 and 80 - FastROM and SlowROM") {
        for (bool fastrom : {false, true}) {
            // $0000-$1FFF -> 8
            REQUIRE(access_cycles(0x001FFF, fastrom) == 8);
            REQUIRE(access_cycles(0x800000, fastrom) == 8);
            REQUIRE(access_cycles(0x801FFF, fastrom) == 8);
            
            // $2000-$3FFF -> 6
            REQUIRE(access_cycles(0x002000, fastrom) == 6);
            REQUIRE(access_cycles(0x003FFF, fastrom) == 6);
            REQUIRE(access_cycles(0x802000, fastrom) == 6);
            REQUIRE(access_cycles(0x803FFF, fastrom) == 6);
            
            // $4000-$41FF -> 12
            REQUIRE(access_cycles(0x004000, fastrom) == 12);
            REQUIRE(access_cycles(0x0041FF, fastrom) == 12);
            REQUIRE(access_cycles(0x804000, fastrom) == 12);
            REQUIRE(access_cycles(0x8041FF, fastrom) == 12);
            
            // $4200-$5FFF -> 6
            REQUIRE(access_cycles(0x004200, fastrom) == 6);
            REQUIRE(access_cycles(0x005FFF, fastrom) == 6);
            REQUIRE(access_cycles(0x804200, fastrom) == 6);
            REQUIRE(access_cycles(0x805FFF, fastrom) == 6);
            
            // $6000-$7FFF -> 8
            REQUIRE(access_cycles(0x006000, fastrom) == 8);
            REQUIRE(access_cycles(0x007FFF, fastrom) == 8);
            REQUIRE(access_cycles(0x806000, fastrom) == 8);
            REQUIRE(access_cycles(0x807FFF, fastrom) == 8);
            
            // $8000-$FFFF (Bank 00 is always 8, Bank 80 is 6 if fastrom else 8)
            REQUIRE(access_cycles(0x008000, fastrom) == 8);
            REQUIRE(access_cycles(0x00FFFF, fastrom) == 8);
            
            REQUIRE(access_cycles(0x808000, fastrom) == (fastrom ? 6 : 8));
            REQUIRE(access_cycles(0x80FFFF, fastrom) == (fastrom ? 6 : 8));
        }
    }

    SECTION("Banks 40 and 7E are always 8") {
        for (bool fastrom : {false, true}) {
            REQUIRE(access_cycles(0x400000, fastrom) == 8);
            REQUIRE(access_cycles(0x40FFFF, fastrom) == 8);
            REQUIRE(access_cycles(0x7E0000, fastrom) == 8);
            REQUIRE(access_cycles(0x7EFFFF, fastrom) == 8);
        }
    }

    SECTION("Bank C0 is 6 or 8 depending on fastrom") {
        REQUIRE(access_cycles(0xC00000, false) == 8);
        REQUIRE(access_cycles(0xC0FFFF, false) == 8);
        REQUIRE(access_cycles(0xC00000, true) == 6);
        REQUIRE(access_cycles(0xC0FFFF, true) == 6);
    }
}
