# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.28)

project(henoSNES
    VERSION 0.0.1
    DESCRIPTION "Open source SNES emulator"
    HOMEPAGE_URL "https://github.com/nickheno/henoSNES"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Export compile_commands.json for clang-tidy and IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(HENO_BUILD_FRONTEND "Build Qt 6 frontend (requires Qt and SDL3)" OFF)
option(HENO_BUILD_GFX "Build graphics backends" OFF)
option(HENO_BUILD_TESTS "Build tests" ON)

# Git version info
include(cmake/GitVersion.cmake)

# Core library
add_subdirectory(core)

# Tools
add_subdirectory(tools/heno-cli)

# Tests
if(HENO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
