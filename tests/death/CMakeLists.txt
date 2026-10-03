# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(scheduler_reentrancy scheduler_reentrancy.cpp)

target_link_libraries(scheduler_reentrancy PRIVATE heno-core)

target_compile_features(scheduler_reentrancy PRIVATE cxx_std_20)

add_test(NAME scheduler_reentrancy COMMAND scheduler_reentrancy)
set_tests_properties(scheduler_reentrancy PROPERTIES WILL_FAIL TRUE)
