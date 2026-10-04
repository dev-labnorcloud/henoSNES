#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 henoSNES contributors
# SPDX-License-Identifier: GPL-3.0-or-later
"""Genera core/cpu/opcode_table.h a partir de docs/refs/65c816-opcodes.md.

Uso (desde cualquier directorio):

    python tools/gen_opcode_table/gen_opcode_table.py

Se ejecuta a mano; la compilación NO lo invoca. Solo usa la biblioteca estándar.
Si alguna validación falla, termina con código 1 y no escribe nada.
La salida es determinista: dos ejecuciones producen un archivo idéntico.
"""

import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
INPUT_PATH = REPO_ROOT / "docs" / "refs" / "65c816-opcodes.md"
OUTPUT_PATH = REPO_ROOT / "core" / "cpu" / "opcode_table.h"

EXPECTED_ROWS = 256
EXPECTED_MNEMONICS = 92

# (token de la tabla de referencia, enumerador de AddrMode), en el orden del enum.
ADDR_MODES = [
    ("i", "Implied"),
    ("A", "Accumulator"),
    ("#", "Immediate"),
    ("a", "Absolute"),
    ("a,X", "AbsoluteX"),
    ("a,Y", "AbsoluteY"),
    ("al", "AbsoluteLong"),
    ("al,X", "AbsoluteLongX"),
    ("(a)", "AbsoluteIndirect"),
    ("(a,X)", "AbsoluteIndexedIndirect"),
    ("d", "Direct"),
    ("d,X", "DirectX"),
    ("d,Y", "DirectY"),
    ("(d)", "DirectIndirect"),
    ("(d,X)", "DirectIndexedIndirect"),
    ("(d),Y", "DirectIndirectIndexed"),
    ("[d]", "DirectIndirectLong"),
    ("[d],Y", "DirectIndirectLongIndexed"),
    ("d,S", "StackRelative"),
    ("(d,S),Y", "StackRelativeIndirectIndexed"),
    ("r", "Relative"),
    ("rl", "RelativeLong"),
    ("s", "Stack"),
    ("xyc", "BlockMove"),
]
MODE_BY_TOKEN = dict(ADDR_MODES)

ROW_PREFIX_RE = re.compile(r"^\|\s*`\$")
OPCODE_RE = re.compile(r"^`\$([0-9A-F]{2})`$")
MNEMONIC_RE = re.compile(r"^[A-Z]{3}$")
BYTES_RE = re.compile(r"^[0-9]+$")

HEADER_COMMENT = (
    "// GENERADO por tools/gen_opcode_table/gen_opcode_table.py a partir de\n"
    "// docs/refs/65c816-opcodes.md. No editar a mano."
)


def parse_rows(text):
    """Devuelve (filas, errores). Cada fila: (opcode, mnemónico, token de modo, bytes, condiciones)."""
    rows = []
    errors = []
    for line_no, line in enumerate(text.splitlines(), start=1):
        if not ROW_PREFIX_RE.match(line):
            continue
        cells = [c.strip() for c in line.strip().strip("|").split("|")]
        if len(cells) < 6:
            errors.append(f"línea {line_no}: se esperaban al menos 6 columnas, hay {len(cells)}")
            continue
        op_cell, mnemonic, mode, bytes_cell, _cycles, conditions = cells[:6]

        m = OPCODE_RE.match(op_cell)
        if not m:
            errors.append(f"línea {line_no}: opcode inválido {op_cell!r}")
            continue
        opcode = int(m.group(1), 16)

        if not MNEMONIC_RE.match(mnemonic):
            errors.append(f"línea {line_no}: mnemónico inválido {mnemonic!r}")
            continue
        if mode not in MODE_BY_TOKEN:
            errors.append(f"línea {line_no}: modo de direccionamiento desconocido {mode!r}")
            continue
        if not BYTES_RE.match(bytes_cell) or not 1 <= int(bytes_cell) <= 4:
            errors.append(f"línea {line_no}: bytes fuera de rango (1-4): {bytes_cell!r}")
            continue

        cond_tokens = [t.strip() for t in conditions.split(",") if t.strip()]
        rows.append((opcode, mnemonic, mode, int(bytes_cell), cond_tokens))
    return rows, errors


def validate(rows):
    errors = []
    if len(rows) != EXPECTED_ROWS:
        errors.append(f"se esperaban {EXPECTED_ROWS} filas, hay {len(rows)}")

    seen = {}
    for opcode, *_ in rows:
        seen[opcode] = seen.get(opcode, 0) + 1
    for opcode in sorted(op for op, n in seen.items() if n > 1):
        errors.append(f"opcode ${opcode:02X} repetido")
    missing = [op for op in range(256) if op not in seen]
    if missing:
        listed = ", ".join(f"${op:02X}" for op in missing)
        errors.append(f"faltan opcodes: {listed}")

    mnemonics = {row[1] for row in rows}
    if len(mnemonics) != EXPECTED_MNEMONICS:
        errors.append(
            f"se esperaban {EXPECTED_MNEMONICS} mnemónicos distintos, hay {len(mnemonics)}"
        )
    return errors


def imm_size(mode, cond_tokens):
    if mode != "#":
        return "None"
    if "m" in cond_tokens:
        return "M"
    if "x" in cond_tokens:
        return "X"
    return "Fixed8"


def render(rows):
    by_opcode = sorted(rows, key=lambda r: r[0])
    mnemonics = sorted({r[1] for r in rows})
    mode_width = max(len(name) for _, name in ADDR_MODES) + 1

    out = []
    out.append("// SPDX-FileCopyrightText: 2026 henoSNES contributors")
    out.append("// SPDX-License-Identifier: GPL-3.0-or-later")
    out.append("")
    out.append(HEADER_COMMENT)
    out.append("")
    out.append("#pragma once")
    out.append("")
    out.append("#include <array>")
    out.append("#include <cstddef>")
    out.append("#include <cstdint>")
    out.append("")
    out.append("namespace heno {")
    out.append("")

    out.append(f"// {len(mnemonics)} mnemónicos, en orden alfabético.")
    out.append("enum class Mnemonic : std::uint8_t {")
    for name in mnemonics:
        out.append(f"    {name},")
    out.append("};")
    out.append("")

    out.append("// Modos de direccionamiento; el comentario indica el token de la tabla de referencia.")
    out.append("enum class AddrMode : std::uint8_t {")
    for token, name in ADDR_MODES:
        out.append(f"    {(name + ',').ljust(mode_width)} // {token}")
    out.append("};")
    out.append("")

    out.append("// Tamaño del operando inmediato: M o X dependen del flag homónimo; Fixed8 es")
    out.append("// siempre 1 byte (REP, SEP). None para los modos que no son Immediate.")
    out.append("enum class ImmSize : std::uint8_t {")
    out.append("    None,")
    out.append("    Fixed8,")
    out.append("    M,")
    out.append("    X,")
    out.append("};")
    out.append("")

    out.append("struct OpcodeInfo {")
    out.append("    Mnemonic mnemonic;")
    out.append("    AddrMode mode;")
    out.append("    std::uint8_t base_bytes;")
    out.append("    ImmSize imm;")
    out.append("};")
    out.append("")

    out.append("// Indexada por opcode. base_bytes corresponde a M = 1 y X = 1.")
    out.append("inline constexpr std::array<OpcodeInfo, 256> kOpcodeTable{{")
    for opcode, mnemonic, mode, nbytes, cond_tokens in by_opcode:
        out.append(
            f"    /* ${opcode:02X} */ {{Mnemonic::{mnemonic}, AddrMode::{MODE_BY_TOKEN[mode]}, "
            f"{nbytes}, ImmSize::{imm_size(mode, cond_tokens)}}},"
        )
    out.append("}};")
    out.append("")

    out.append("constexpr const char* mnemonic_name(Mnemonic m) noexcept {")
    out.append(f"    constexpr std::array<const char*, {len(mnemonics)}> kNames{{{{")
    for name in mnemonics:
        out.append(f'        "{name}",')
    out.append("    }};")
    out.append("    const auto index = static_cast<std::size_t>(m);")
    out.append('    return index < kNames.size() ? kNames[index] : "???";')
    out.append("}")
    out.append("")

    out.append("// Longitud total en bytes (opcode incluido) según el estado de M y X")
    out.append("// (m8 / x8 = true: registro de 8 bits).")
    out.append(
        "constexpr std::uint8_t instruction_length(std::uint8_t opcode, bool m8, bool x8) noexcept {"
    )
    out.append("    const OpcodeInfo& info = kOpcodeTable[opcode];")
    out.append("    std::uint8_t length = info.base_bytes;")
    out.append("    if (info.imm == ImmSize::M && !m8) {")
    out.append("        ++length;")
    out.append("    }")
    out.append("    if (info.imm == ImmSize::X && !x8) {")
    out.append("        ++length;")
    out.append("    }")
    out.append("    return length;")
    out.append("}")
    out.append("")
    out.append("} // namespace heno")
    out.append("")
    return "\n".join(out)


def main():
    try:
        text = INPUT_PATH.read_text(encoding="utf-8")
    except OSError as exc:
        print(f"error: no se puede leer {INPUT_PATH}: {exc}", file=sys.stderr)
        return 1

    rows, errors = parse_rows(text)
    errors += validate(rows)
    if errors:
        for msg in errors:
            print(f"error: {msg}", file=sys.stderr)
        print(f"error: validación fallida; no se escribe {OUTPUT_PATH}", file=sys.stderr)
        return 1

    content = render(rows)
    OUTPUT_PATH.parent.mkdir(parents=True, exist_ok=True)
    with open(OUTPUT_PATH, "w", encoding="utf-8", newline="\n") as f:
        f.write(content)
    print(f"escrito {OUTPUT_PATH} ({len(rows)} opcodes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
