# AGENTS.md — henoSNES

Este archivo gobierna a todo agente de IA que trabaje en este repositorio (Antigravity u otro).
Léelo completo antes de cualquier acción. La especificación completa está en `docs/SPEC.md`.

## Qué es este proyecto

Emulador open source (GPL-3.0-or-later) de Super Nintendo, multiplataforma (Windows, Linux, macOS),
con núcleo en C++20 sin dependencias de UI, frontend Qt 6, overlay ImGui, plataforma SDL3 y cuatro
backends gráficos intercambiables (Vulkan 1.3, Direct3D 12, Metal 3, OpenGL 3.3). Ver `docs/SPEC.md` §4-§6.

## Reglas legales duras (no negociables)

1. NUNCA descargar, buscar, enlazar, indexar ni mencionar fuentes de ROMs comerciales.
   Solo se usan las ROMs homebrew con licencia libre listadas en `tests/roms/MANIFEST.json`.
2. NUNCA incluir BIOS, firmware ni microcódigo propietario. Los coprocesadores se implementan en HLE
   o cargan el archivo que el usuario provea.
3. NUNCA copiar código de proyectos con licencia incompatible con GPLv3 ni código filtrado.
   Todo archivo portado (bsnes, Snes9x, Mesen, libretro) se registra en `THIRD_PARTY_NOTICES.md`
   con URL de origen, commit y licencia.
4. El nombre "henoSNES" usa "SNES" solo de forma descriptiva. No reproducir logotipos, tipografías
   ni diseños de Nintendo en ningún asset.
5. El autor y Signed-off-by de los commits deben corresponder al humano responsable del merge, no a la IA. El agente debe usar la identidad configurada en el entorno local.

## Convenciones de código

- C++20. Clang 17 / GCC 13 / MSVC 19.38. CMake 3.28 + Ninja + vcpkg (modo manifest).
- `core/` no incluye Qt, SDL, Vulkan ni ninguna API de SO. Produce framebuffer + PCM; nada más.
- Sin excepciones C++ en `core/`: códigos de retorno y `std::expected`.
- Todo acceso a memoria emulada pasa por el bus (trazabilidad para depurador y run-ahead).
- clang-format y clang-tidy obligatorios; CI rechaza PRs que no pasen.
- Cabecera SPDX en cada archivo (REUSE 3.0).
- Cada decisión no trivial tiene un ADR en `docs/adr/NNNN-titulo.md` ANTES de implementarse.
- Sin dependencias nuevas sin ADR aprobado por un humano.

## Definición de terminado (por PR)

- Un PR = una tarea de `docs/BACKLOG.md`. Máximo 800 líneas netas salvo tablas generadas.
- Build verde en los tres SO.
- Pruebas unitarias nuevas para el código nuevo.
- Suite de golden frames (`tests/golden`) sin regresiones. Una regresión bloquea el merge
  aunque el agente la considere una mejora.
- Descripción del PR enlaza al ADR y adjunta evidencia (hashes, capturas, benchmarks).

## Flujo de trabajo del agente

1. Leer `AGENTS.md`, `docs/SPEC.md` y `docs/BACKLOG.md`.
2. Generar un plan y guardarlo como artefacto para revisión humana antes de escribir código.
3. Trabajar en rama `feat/<id-backlog>`.
4. Ejecutar pruebas locales y CI.
5. Abrir PR. Un segundo agente con modelo distinto revisa. Un humano aprueba el merge.

## Asignación de modelos (orientativa)

- Núcleo (CPU, PPU, APU, scheduler, serialización), API en C, backends gráficos: Claude Opus 4.6 (Thinking), modo Planning.
- Frontend Qt, temas, i18n, biblioteca: Gemini 3.1 Pro, modo Fast.
- Pruebas, CI, empaquetado, documentación: Gemini Flash, modo Fast.
- Revisión de licencias: Claude Sonnet 4.6, modo Planning.

## Qué NO hacer

- No "mejorar" la precisión de emulación sin un test ROM o golden frame que lo demuestre.
- No añadir funciones fuera de la fase actual del roadmap (`docs/SPEC.md` §7).
- No tocar `core/` desde una tarea de frontend ni viceversa sin ADR.
- No usar `localStorage`, telemetría ni conexiones de red no documentadas en la SPEC.
