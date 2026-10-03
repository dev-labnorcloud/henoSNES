// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdlib>

#define HENO_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::abort(); \
        } \
    } while (0)
