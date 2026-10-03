# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

add_library(heno-core STATIC
    api/heno.cpp
    serial/serializer.cpp
    sched/scheduler.cpp
    cart/cart.cpp
    bus/bus.cpp
)

target_include_directories(heno-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(heno-core PUBLIC cxx_std_20)

# Core must NOT depend on any UI, OS, or graphics library.
# It produces framebuffer + PCM only.
