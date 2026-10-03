<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Playbook: Sesión Estándar de Desarrollo

Este playbook guía a los agentes de IA (y desarrolladores) a través del flujo de trabajo estándar por sesión en el repositorio henoSNES, según lo definido en `AGENTS.md` y `docs/SPEC.md` §8.

---

## 1. Fase Previa (Lectura y Comprensión)

Antes de realizar cualquier modificación en el código:

1. **Lectura obligatoria:**
   - [AGENTS.md](../../AGENTS.md): Reglas legales duras, convenciones de código y asignación de modelos.
   - [docs/SPEC.md](../../docs/SPEC.md): Especificación técnica del área a trabajar.
   - [docs/BACKLOG.md](../../docs/BACKLOG.md): Tarea específica a implementar (`<id-backlog>`).

2. **Verificación legal:**
   - Confirmar que la tarea no involucra ROMs comerciales, BIOS propietaria, microcódigo ni dependencias incompatibles con GPLv3.
   - Si se porta código de terceros permitido (bsnes, Snes9x, libretro), identificar URL, commit y licencia para `THIRD_PARTY_NOTICES.md`.

---

## 2. Generación del Plan

1. **Crear artefacto de plan:**
   - Generar un documento estructurado como artefacto para revisión humana antes de escribir código.
   - Incluir:
     - Alcance exacto de la tarea.
     - Decisiones arquitectónicas no explícitas (identificando si requieren ADR nuevo).
     - Archivos a crear, modificar o eliminar.
     - Pruebas unitarias o de integración a añadir.
     - Criterios de aceptación y gate de la tarea.
2. **Esperar aprobación humana** antes de proceder a la implementación.

---

## 3. Implementación

1. **Rama de trabajo:**
   ```bash
   git checkout -b feat/<id-backlog>
   ```
2. **Convenciones estrictas:**
   - C++20 con CMake 3.28+ y Ninja.
   - `core/` no incluye librerías de UI, OS, gráficos ni excepciones C++ (`std::expected` y códigos de retorno).
   - Todo acceso a memoria emulada debe pasar por el bus.
   - Cabecera SPDX en cada archivo nuevo.
   - Formatear con `.clang-format` y verificar con `.clang-tidy`.
   - Tamaño máximo recomendado por PR: 800 líneas netas (salvo tablas generadas).

---

## 4. Pruebas y Validación Local

1. **Compilación:**
   ```bash
   cmake --preset <tu-preset>
   cmake --build --preset <tu-preset>
   ```
2. **Ejecución de pruebas:**
   ```bash
   ctest --preset <tu-preset> --output-on-failure
   ```
3. **Validación de regresión:**
   - Ejecutar suite de golden frames si aplica: ninguna regresión gráfica permitida.

---

## 5. Apertura de Pull Request

1. **Commits con DCO:**
   - Seguir formato Conventional Commits con `Signed-off-by:` (`git commit -s`).
   - Ejemplo: `feat(cpu): implement 65C816 ADC opcode (P1-002)`
2. **Crear PR hacia `main`:**
   - Título descriptivo con prefijo convencional.
   - Cuerpo del PR debe incluir:
     - Enlace al ID del backlog correspondiente.
     - Enlace al ADR aplicable (o justificación si no aplica).
     - Evidencia de pruebas (logs, hashes de frames, capturas de pantalla, benchmarks).
3. **Revisión por segundo modelo:**
   - Un agente con modelo distinto al implementador revisa el PR.
4. **Aprobación final:**
   - Un humano aprueba y ejecuta el merge a `main`.
