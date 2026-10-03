<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Backlog — henoSNES

Cada tarea corresponde a un PR. Máximo 800 líneas netas por PR salvo tablas generadas.
Ver `docs/SPEC.md` §7 para las fases completas.

## Fase 0 — Cimientos

| ID      | Descripción                                                              | Gate                                                    | Estado    |
|---------|---------------------------------------------------------------------------|---------------------------------------------------------|-----------|
| P0-001  | Estructura de carpetas y READMEs                                        | Directorios creados con README.md                       | EN CURSO  |
| P0-002  | CMakeLists.txt raíz + CMakePresets.json multiplataforma                 | Configura en Windows, Linux y macOS                     | EN CURSO  |
| P0-003  | vcpkg.json modo manifest con dependencias                               | vcpkg install exitoso en 3 SO                           | EN CURSO  |
| P0-004  | `core/api/heno.h` — API C con funciones declaradas (stubs)              | Compila sin errores                                     | EN CURSO  |
| P0-005  | Tests de ABI (Catch2) para heno.h                                       | Tests de sizeof y enum values pasan                     | EN CURSO  |
| P0-006  | `tools/heno-cli/main.cpp` — imprime versión y hash de commit            | `heno-cli --version` funciona                           | EN CURSO  |
| P0-007  | GitHub Actions CI: build + test en Windows, Linux, macOS (x64 + arm64)  | CI verde en los 4 jobs                                  | EN CURSO  |
| P0-008  | Archivos de gobierno: LICENSE, LEGAL.md, CONTRIBUTING.md, SECURITY.md   | Archivos presentes y completos                          | EN CURSO  |
| P0-009  | THIRD_PARTY_NOTICES.md con plantilla                                    | Plantilla de entrada documentada                        | EN CURSO  |
| P0-010  | .clang-format y .clang-tidy configurados                                | clang-format no modifica archivos existentes            | EN CURSO  |
| P0-011  | Configuración REUSE 3.0 (.reuse/dep5 + cabeceras SPDX)                 | Todos los archivos tienen identificador de licencia     | EN CURSO  |
| P0-012  | ADR-0001: Licencia y alcance legal                                      | ADR en formato MADR                                     | EN CURSO  |
| P0-013  | ADR-0002: Arquitectura por capas                                        | ADR en formato MADR                                     | EN CURSO  |
| P0-014  | ADR-0003: Contrato de la API C                                          | ADR en formato MADR                                     | EN CURSO  |
| P0-015  | docs/BACKLOG.md con tareas de fases 0 y 1                               | Backlog documentado                                     | EN CURSO  |

## Fase 1 — Núcleo mínimo

| ID      | Descripción                                                              | Gate                                                    | Estado |
|---------|--------------------------------------------------------------------------|---------------------------------------------------------|--------|
| P1-000  | Verificar docs/refs/65c816-opcodes.md contra fuente primaria (Eyes & Lichty, apéndice de opcodes) antes de implementar el intérprete. 256 filas pendientes. | 256 filas verificadas | TODO |
| P1-001  | CPU 65C816 — decodificador de opcodes y modos de direccionamiento       | Todos los opcodes decodifican correctamente             | TODO   |
| P1-002  | CPU 65C816 — ALU y registros (acumulador, índices, status)              | Tests unitarios por opcode                              | TODO   |
| P1-003  | CPU 65C816 — interrupciones (NMI, IRQ, BRK, COP, RESET)                | Tests de vectores y timing                              | TODO   |
| P1-004  | CPU 65C816 — conteo de ciclos por acceso al bus                         | Ciclos coinciden con documentación de referencia        | TODO   |
| P1-005  | Scheduler cooperativo — relojes independientes CPU/PPU/APU              | Sincronización en puntos de acceso compartido           | TODO   |
| P1-006  | Bus de memoria — mapeo LoROM y HiROM                                    | Lecturas/escrituras en direcciones correctas            | TODO   |
| P1-007  | Bus de memoria — WRAM, registros I/O, open bus                          | Tests de acceso a memoria                               | TODO   |
| P1-008  | PPU — modo 0 (4 backgrounds, 4 colores cada uno)                       | Golden frames de test ROM                               | TODO   |
| P1-009  | PPU — modo 1 (3 backgrounds, 16/16/4 colores)                          | Golden frames de test ROM                               | TODO   |
| P1-010  | PPU — sprites (OAM, prioridad, flipping, tamaños)                      | Golden frames de test ROM                               | TODO   |
| P1-011  | PPU — scrolling y ventanas (window 1/2, color math)                    | Golden frames de test ROM                               | TODO   |
| P1-012  | APU — SPC700 CPU completa                                               | 100% en test ROMs de SPC700                             | TODO   |
| P1-013  | APU — S-DSP (8 voces, ADSR, BRR, eco, ruido)                          | Audio reconocible en ROMs homebrew                      | TODO   |
| P1-014  | Cartridge — SRAM con battery save/load                                  | Guardado persiste entre sesiones                        | TODO   |
| P1-015  | Serialización — estados básicos (CPU + PPU + APU + RAM)                 | Serialize → deserialize reproduce frame idéntico        | TODO   |
| P1-016  | heno-cli headless — volcado de frames a PNG                             | Genera PNG idéntico al golden frame                     | TODO   |
| P1-017  | Test ROMs de CPU — suite completa de la comunidad                       | 100% en verde                                           | TODO   |
| P1-018  | Test ROMs de SPC700 — suite completa de la comunidad                    | 100% en verde                                           | TODO   |
| P1-019  | Golden frames — 20 ROMs homebrew libres × 300 frames                    | Hashes SHA-256 estables                                 | TODO   |
| P1-020  | ADR-0003+: Estrategia de scheduler cooperativo                          | ADR en formato MADR                                     | TODO   |
