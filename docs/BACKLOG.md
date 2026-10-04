<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Backlog — henoSNES

Cada tarea corresponde a un PR. Máximo 800 líneas netas por PR salvo tablas generadas.
Ver `docs/SPEC.md` §7 para las fases completas.

## Fase 0 — Cimientos

| ID      | Descripción                                                              | Gate                                                    | Estado    |
|---------|---------------------------------------------------------------------------|---------------------------------------------------------|-----------|
| P0-001  | Estructura de carpetas y READMEs                                        | Directorios creados con README.md                       | RESUELTO  |
| P0-002  | CMakeLists.txt raíz + CMakePresets.json multiplataforma                 | Configura en Windows, Linux y macOS                     | RESUELTO  |
| P0-003  | vcpkg.json modo manifest con dependencias                               | vcpkg install exitoso en 3 SO                           | RESUELTO  |
| P0-004  | `core/api/heno.h` — API C con funciones declaradas (stubs)              | Compila sin errores                                     | RESUELTO  |
| P0-005  | Tests de ABI (Catch2) para heno.h                                       | Tests de sizeof y enum values pasan                     | RESUELTO  |
| P0-006  | `tools/heno-cli/main.cpp` — imprime versión y hash de commit            | `heno-cli --version` funciona                           | RESUELTO  |
| P0-007  | GitHub Actions CI: build + test en Windows, Linux, macOS (x64 + arm64)  | CI verde en los 4 jobs                                  | RESUELTO  |
| P0-008  | Archivos de gobierno: LICENSE, LEGAL.md, CONTRIBUTING.md, SECURITY.md   | Archivos presentes y completos                          | RESUELTO  |
| P0-009  | THIRD_PARTY_NOTICES.md con plantilla                                    | Plantilla de entrada documentada                        | RESUELTO  |
| P0-010  | .clang-format y .clang-tidy configurados                                | clang-format no modifica archivos existentes            | RESUELTO  |
| P0-011  | Configuración REUSE 3.0 (.reuse/dep5 + cabeceras SPDX)                 | Todos los archivos tienen identificador de licencia     | RESUELTO  |
| P0-012  | ADR-0001: Licencia y alcance legal                                      | ADR en formato MADR                                     | RESUELTO  |
| P0-013  | ADR-0002: Arquitectura por capas                                        | ADR en formato MADR                                     | RESUELTO  |
| P0-014  | ADR-0003: Contrato de la API C                                          | ADR en formato MADR                                     | RESUELTO  |
| P0-015  | docs/BACKLOG.md con tareas de fases 0 y 1                               | Backlog documentado                                     | RESUELTO  |
| P0-016  | Deuda técnica: CI runner macOS x64 Intel (`macos-13`)                  | Aislado en workflow manual (.github/workflows/ci-macos-x64.yml) | RESUELTO  |

### Registro de Deuda Técnica (Fase 0)

- **DT-001 (Runner macOS Intel x86_64 manual):** El job `macos-13` en GitHub Actions opera bajo capacidad limitada y alta congestión de cola por la obsolescencia progresiva de hardware Intel en los pools públicos de GitHub. El job ha sido aislado en el workflow independiente `.github/workflows/ci-macos-x64.yml` con disparador manual (`workflow_dispatch`), permitiendo compilar x86_64 bajo demanda sin bloquear el CI estándar en cada PR/push. Revisar en 2027 si GitHub ofrece runners Intel más modernos (`macos-15-intel`) o descontinuar soporte nativo x86_64 a favor de Rosetta 2 / binarios universales.

## Fase 1 — Núcleo mínimo

| ID      | Descripción                                                              | Gate                                                    | Estado |
|---------|--------------------------------------------------------------------------|---------------------------------------------------------|--------|
| P1-000  | Verificar docs/refs/65c816-opcodes.md contra fuente primaria (hoja de datos WDC W65C816S 2024-03-13, Tablas 5-4 y 5-7) antes de implementar el intérprete. 256 filas verificadas. | 256 filas verificadas | RESUELTO |
| P1-001a | CPU 65C816 — tabla de decodificación, estado y despacho (ADR-0004)      | Los 256 opcodes decodifican correctamente en los 5 modos | RESUELTO |
| P1-001b | CPU 65C816 — modos de direccionamiento y direcciones efectivas          | Tests por modo, incluidos los casos de envolvimiento    | TODO   |
| P1-002  | CPU 65C816 — ALU y registros (acumulador, índices, status)              | Tests unitarios por opcode                              | TODO   |
| P1-003  | CPU 65C816 — interrupciones (NMI, IRQ, BRK, COP, RESET)                | Tests de vectores y timing                              | TODO   |
| P1-004  | CPU 65C816 — conteo de ciclos por acceso al bus                         | Ciclos coinciden con documentación de referencia        | TODO   |
| P1-005  | Scheduler cooperativo — relojes independientes CPU/PPU/APU (ver ADR-0005) | Sincronización en puntos de acceso compartido           | RESUELTO |
| P1-006  | Bus de memoria — mapeo LoROM y HiROM                                    | Lecturas/escrituras en direcciones correctas            | RESUELTO |
| P1-007  | Bus de memoria — WRAM, registros I/O, open bus                          | Tests de acceso a memoria                               | RESUELTO |
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
| P1-020  | ADR-0005: Estrategia de scheduler cooperativo (+ revisión de ADR-0004, Gate A) | ADR en formato MADR                                     | RESUELTO |
| P1-021  | ADR: formato binario de serialización versionado (pendiente declarado en ADR-0005) | ADR en formato MADR                                     | TODO   |
| P1-022  | Selección de ROMs de prueba: test ROMs de CPU y SPC700 de la comunidad y 20 homebrew libres, con licencia verificada y registradas en tests/roms/MANIFEST.json (requisito de P1-017, P1-018 y P1-019) | Manifest con entradas y licencias verificadas           | TODO   |

### Registro de Deuda Técnica (Fase 1)

- **DT-002 (Licencia de documentación):** [RESUELTO] `.reuse/dep5` asigna ahora `GPL-3.0-or-later` a `docs/adr/*.md`, coherente con las cabeceras de los ADR; `docs/refs/*.md` se mantiene en `CC-BY-4.0`.
- **DT-003 (SPEC sin SPDX):** [RESUELTO] `docs/SPEC.md` tiene ahora cabecera SPDX `GPL-3.0-or-later`.
- **DT-004 (Formato sin verificar):** el CI no ejecuta clang-format, aunque AGENTS.md indica que se rechazan PRs que no lo pasan; tampoco está disponible en el entorno local de desarrollo.
- **DT-005 (Advertencias del compilador):** el proyecto no activa advertencias (-Wall/-Wextra, /W4) ni las trata como errores; detectado en P1-006 (includes faltantes que MSVC no reporta).
- **DT-006 (MSVC sin /EHsc en pruebas):** las pruebas se compilan con la advertencia C4530; Catch2 usa excepciones internamente, por lo que una prueba que falla podría no liberar correctamente sus recursos.
- **DT-007 (Referencia de quirks sin verificar):** [RESUELTO] docs/refs/65c816-quirks.md verificada contra fuentes primarias; los puntos marcados [verificar] quedan para P1-001 a P1-004.
- **DT-008 (.gitattributes sin regla para *.py):** los scripts de Python dependen de core.autocrlf para sus finales de línea; agregar "*.py text eol=lf" en un PR de configuración.
