# P-00 · Bootstrap de henoSNES

**Modelo:** Claude Opus 4.6 (Thinking) · **Modo:** Planning · **Workspace:** carpeta `henosnes/` con `AGENTS.md` y `docs/SPEC.md` ya presentes.

---

Lee `AGENTS.md` y `docs/SPEC.md` completos antes de hacer nada. Confirma en tu plan que
entendiste las reglas legales duras de la sección 2 y las convenciones de la sección 6.

Objetivo de esta sesión: dejar el repositorio compilando en Windows, Linux y macOS con un
ejecutable `heno-cli --version`, sin implementar todavía ninguna emulación.

Tareas:

1. Crea la estructura de carpetas exacta de `docs/SPEC.md` §6 (árbol `henosnes/`), con un
   `README.md` mínimo en cada carpeta que explique su propósito en una línea.
2. `CMakeLists.txt` raíz + `CMakePresets.json` con presets `windows-msvc`, `linux-clang`,
   `macos-arm64`, `macos-x64`, cada uno con variantes Debug y Release.
3. `vcpkg.json` en modo manifest con las dependencias de la tabla de §6 (Qt 6.7, SDL3, glslang,
   SPIRV-Cross, libsamplerate, libarchive, zstd, Catch2, enet). Fija versiones con `builtin-baseline`.
4. `core/api/heno.h`: cabecera de la API en C con las funciones declaradas (sin implementar) según §4
   decisión 4. Agrega una prueba Catch2 que verifique el ABI (tamaño de structs, enum values).
5. `tools/heno-cli/main.cpp`: imprime versión y hash de commit.
6. `.github/workflows/ci.yml`: compila los cuatro presets en Release, ejecuta pruebas, sube
   `heno-cli` como artefacto. Usa ccache/sccache.
7. Archivos de gobierno: `LICENSE` (GPL-3.0-or-later), `LEGAL.md` (política de ROMs y marcas según
   §2), `THIRD_PARTY_NOTICES.md` (vacío con plantilla), `CONTRIBUTING.md` con cláusula DCO,
   `SECURITY.md`, `.clang-format`, `.clang-tidy`, configuración REUSE.
8. `docs/adr/0001-licencia-y-alcance-legal.md` y `docs/adr/0002-arquitectura-por-capas.md`
   en formato MADR, derivados de §2 y §4.
9. `docs/BACKLOG.md` con las tareas de las fases 0 y 1 de §7, cada una con ID, descripción y gate.

Gate de aceptación: CI verde en los tres SO con el artefacto `heno-cli` descargable.

Entrega el plan primero y espera aprobación antes de crear archivos. Al terminar, resume qué
decisiones tomaste que no estaban explícitas en la SPEC para que las registre como ADR o las corrija.
