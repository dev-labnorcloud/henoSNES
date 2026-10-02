# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later

# Determine git commit hash for version string
find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" describe --always --dirty --abbrev=12
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE HENO_GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_RESULT
    )
    if(NOT GIT_RESULT EQUAL 0)
        set(HENO_GIT_HASH "unknown")
    endif()
else()
    set(HENO_GIT_HASH "unknown")
endif()

message(STATUS "henoSNES git hash: ${HENO_GIT_HASH}")
