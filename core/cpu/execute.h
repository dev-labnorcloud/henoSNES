// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "cpu/opcode_table.h"
#include "cpu/operand.h"

namespace heno {

template <Mnemonic Mn>
struct Exec {
    template <bool E, bool M8, bool X8, class C>
    static void run(C& /*cpu*/, const Operand& /*operand*/) noexcept {}
};

} // namespace heno
