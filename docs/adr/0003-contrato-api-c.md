<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-0003: Contrato de la API C — structs, errores y versionado

## Estado

Aceptado

## Fecha

2026-10-02

## Contexto

La API C (`heno.h`) es el contrato entre el núcleo de emulación y todos los consumidores
(frontend, CLI, futuro core libretro). Las decisiones sobre layout de structs, manejo de
errores y versionado afectan la estabilidad del ABI y la capacidad de evolución.

Estas decisiones no estaban explícitas en SPEC.md y se registran aquí por requisito de AGENTS.md.

## Decisión

### Layout de structs

- Todos los structs usan tipos de tamaño fijo (`uint8_t`, `uint16_t`, `uint32_t`, etc.).
- Los enums se declaran como `typedef enum` (tamaño `int` en C, estable en la práctica).
- Se incluyen campos `_pad` explícitos para controlar la alineación y documentar el layout.
- Los tamaños de struct son verificados por tests de ABI (`tests/unit/test_abi.cpp`).
- Los tamaños esperados se documentan para 32-bit y 64-bit por separado.

### Manejo de errores

- Todas las funciones de la API retornan `HenoError`.
- `HENO_OK` = 0 (éxito). Valores > 0 son errores.
- Valores de enum asignados explícitamente (no dependen del compilador).
- `HENO_ERROR_UNKNOWN` = 255, reservando espacio para errores futuros.
- Internamente en C++20, el núcleo usa `std::expected<T, HenoError>` y convierte
  a `HenoError` en la frontera de la API C.

### Versionado de API

- Versión semántica: MAJOR.MINOR.PATCH.
- La API se declara como v0.1.0 (inestable) hasta fase 3.
- Cualquier cambio de layout de struct o valor de enum requiere bump de MAJOR.
- Funciones nuevas pueden añadirse con bump de MINOR.
- La versión se consulta con `heno_api_version()` (packed uint32) y
  `heno_version()` (string con hash de commit).

## Consecuencias

- ✅ ABI verificable automáticamente en cada PR.
- ✅ Los consumidores pueden verificar compatibilidad en runtime con `heno_api_version()`.
- ⚠️ Los campos `_pad` desperdician algunos bytes pero aseguran alineación predecible.
- ⚠️ La API v0 puede cambiar; los consumidores deben estar preparados para recompilar.

## Documentos relacionados

- `core/api/heno.h` — Definición de la API
- `tests/unit/test_abi.cpp` — Tests de estabilidad de ABI
- ADR-0002 — Arquitectura por capas
