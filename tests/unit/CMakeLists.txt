# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(heno-tests
    test_abi.cpp
    test_scheduler.cpp
    test_access_speed.cpp
    test_cartridge.cpp
    test_bus.cpp
    test_wram.cpp
    test_cpu_decode.cpp
    test_opcode_table.cpp
)

target_link_libraries(heno-tests
    PRIVATE
        heno-core
        Catch2::Catch2WithMain
)

target_compile_definitions(heno-tests PRIVATE
    HENO_VERSION="${PROJECT_VERSION}"
)

include(CTest)
include(Catch)
catch_discover_tests(heno-tests)
