# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(heno-tests
    test_abi.cpp
    test_serialize.cpp
    test_scheduler.cpp
    test_bus.cpp
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
