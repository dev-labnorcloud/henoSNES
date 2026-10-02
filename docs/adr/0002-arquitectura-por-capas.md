<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-0002: Arquitectura por capas con API en C

## Estado

Aceptado

## Fecha

2026-10-02

## Contexto

El emulador necesita soportar múltiples frontends (Qt desktop, CLI headless, futuro libretro core),
múltiples backends gráficos (Vulkan, D3D12, Metal, OpenGL) y múltiples plataformas (Windows, Linux,
macOS, futuro Android). La arquitectura debe permitir que el núcleo de emulación sea completamente
independiente de la interfaz de usuario, la plataforma y la API gráfica.

## Decisión

Arquitectura de cuatro capas con separación estricta:

```
┌─────────────────────────────────────────────┐
│ Capa 4: Frontend (Qt 6 Widgets + QML)       │
│         ImGui overlay (HUD, debugger)       │
├─────────────────────────────────────────────┤
│ Capa 3: Plataforma (SDL3)                   │
│         Ventana, audio, input, displays     │
├─────────────────────────────────────────────┤
│ Capa 2: Backends gráficos (heno-gfx)       │
│         Vulkan │ D3D12 │ Metal │ OpenGL     │
├─────────────────────────────────────────────┤
│ Capa 1: Núcleo (libheno-core)              │
│         CPU, PPU, APU, Cart, Serial         │
│         API C estable (heno.h)              │
└─────────────────────────────────────────────┘
```

**Reglas de dependencia:**
- El núcleo (capa 1) **no incluye** Qt, SDL, Vulkan ni ninguna API de SO.
- El núcleo produce framebuffer (BGR555, hasta 512×478) + PCM (32040 Hz, 16-bit stereo).
- La comunicación entre frontend y núcleo es exclusivamente a través de `heno.h` (C ABI).
- Los backends gráficos se seleccionan en tiempo de ejecución.
- El hilo de emulación es independiente del hilo de UI.

**API en C (`heno.h`):**
- Funciones: init/deinit, load/unload ROM, run_frame, serialize/deserialize, callbacks, memory access.
- Error handling: enum `HenoError` con códigos numéricos estables.
- Callbacks: video_refresh, audio_sample, input_poll, input_state, log.
- Versionada con HENO_API_VERSION_MAJOR/MINOR/PATCH.

## Alternativas consideradas

1. **API C++ con clases virtuales:** Más idiomática pero rompe ABI entre compiladores. Descartada.
2. **Header-only library:** Imposible separar frontend de núcleo. Descartada.
3. **API libretro directa:** Acopla al formato libretro desde el inicio. Se hará un adaptador en fase 3.

## Consecuencias

- ✅ El núcleo es testeable headless (`heno-cli`) sin ninguna dependencia gráfica.
- ✅ Múltiples frontends pueden reutilizar el mismo núcleo.
- ✅ Futuro core libretro es un adaptador delgado sobre `heno.h`.
- ✅ Portabilidad maximizada: compilar core para cualquier plataforma con compilador C++20.
- ⚠️ La API C requiere conversión manual de tipos en el frontend C++.
- ⚠️ Cualquier cambio en `heno.h` es un cambio de ABI que necesita pruebas de compatibilidad.

## Documentos relacionados

- `core/api/heno.h` — Definición de la API
- `tests/unit/test_abi.cpp` — Tests de estabilidad de ABI
- SPEC.md §4 — Decisiones de diseño clave
