# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(heno-cli main.cpp)

target_link_libraries(heno-cli PRIVATE heno-core)

target_compile_definitions(heno-cli PRIVATE
    HENO_VERSION="${PROJECT_VERSION}"
    HENO_GIT_HASH="${HENO_GIT_HASH}"
)

install(TARGETS heno-cli RUNTIME DESTINATION bin)
