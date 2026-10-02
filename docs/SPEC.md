# SPEC — Emulador SNES Open Source Multiplataforma

Oct 2, 2026 · @Arquitecto CRM

## 1. Resumen ejecutivo

**Proyecto*: henoSNES*** — emulador de Super Nintendo (SNES / Super Famicom) open source, multiplataforma (Windows, Linux, macOS; Android y Steam Deck en fase 2), con énfasis en tres atributos: precisión de emulación, experiencia de usuario amigable y un pipeline de renderizado moderno que aproveche tanto GPUs recientes (Vulkan 1.3, DirectX 12, Metal 3, HDR, VRR, upscaling) como hardware antiguo (OpenGL 3.3 / GLES 3.0), con paridad funcional en AMD y NVIDIA (e Intel Arc).

El diferenciador no es un nuevo núcleo de emulación desde cero: el núcleo se construye sobre bases ya probadas y licenciadas en forma compatible (bsnes/higan como referencia ciclo-exacta; Snes9x como referencia de rendimiento), y la inversión del proyecto se concentra en el **frontend**, la **capa gráfica**, la **personalización** y la **portabilidad**. Esto reduce de años a meses el camino a un producto estable y evita reinventar décadas de ingeniería inversa documentada.

| Dimensión | Decisión |
| --- | --- |
| Licencia | GPL-3.0-or-later (compatible con bsnes GPLv3 y shaders slang de libretro) |
| Lenguaje principal | C++20 (núcleo y backends) + Rust opcional para herramientas CLI |
| Plataformas v1.0 | Windows 10+, Linux (glibc 2.31+), macOS 12+ (x86\_64 y Apple Silicon) |
| GPU mínima | OpenGL 3.3 / GLES 3.0; ruta moderna Vulkan 1.3, D3D12 FL 12\_0, Metal 3 |
| Público objetivo | Jugadores casuales (modo simple), entusiastas (modo avanzado), desarrolladores homebrew (modo debug) |
| Modelo de desarrollo | Agentic-first: construido con Antigravity, con especificación ejecutable y pruebas de regresión automáticas |

**Métricas de éxito v1.0:** compatibilidad ≥ 99 % en el set de pruebas de ROMs homebrew y de dominio público; latencia de entrada ≤ 1 frame (16,7 ms) en modo run-ahead; 60 fps estables con shaders CRT en una GPU integrada de 2018 (Intel UHD 620 / Vega 8); instalación a primer juego en menos de 60 segundos.

## 2. Marco legal y alcance

El proyecto se mantiene dentro del precedente establecido para emuladores open source: se emula el hardware, no se distribuye software de terceros. La SNES no requiere BIOS propietaria, lo que elimina el principal riesgo legal de otros sistemas.

**Lo que el proyecto hace:**

- Emula la CPU 65C816, PPU, APU (SPC700 + DSP) y chips de expansión (SA-1, SuperFX, DSP-1/2/3/4, Cx4, S-DD1, SPC7110, ST010/011) mediante implementación propia o código GPL de terceros debidamente atribuido.
- Carga archivos de ROM que el usuario ya posee en su disco local (formatos .sfc, .smc, .fig, .swc, .bs, comprimidos en .zip/.7z).
- Incluye, para pruebas y demostración, únicamente ROMs homebrew con licencia libre explícita (por ejemplo, del catálogo de SNESdev con licencia MIT/CC) y test ROMs de la comunidad.

**Lo que el proyecto NO hace (reglas duras para el agente de desarrollo):**

- No incluye, enlaza, descarga, indexa ni sugiere fuentes de ROMs comerciales. No existe función de "buscar ROM en internet" ni scraper de sitios de descarga.
- No incluye imágenes de BIOS, firmware ni microcódigo propietario (los chips DSP-1..4, ST010/011 y Cx4 usan reimplementaciones HLE o requieren que el usuario provea el archivo si opta por LLE).
- No incorpora código de emuladores con licencias no compatibles con GPLv3 ni código filtrado de Nintendo.
- Las carátulas y metadatos de la biblioteca se obtienen solo mediante hash de ROM contra bases de datos comunitarias abiertas (No-Intro DAT, libretro-database) y el usuario decide si activa la descarga de arte desde servicios de terceros bajo sus propios términos.
- Marcas registradas: el nombre henoSNES usa "SNES" de forma descriptiva (indica compatibilidad, como ya hacen Snes9x o SNESGT), nunca como marca propia; el logo y los materiales no reproducen logotipos, tipografías ni diseños de Nintendo, y la documentación evita "Nintendo", "Super Nintendo" y "Super Famicom" salvo en frases descriptivas ("compatible con juegos de Super Nintendo").

**Documentos obligatorios en el repositorio:** `LICENSE` (GPL-3.0-or-later), `LEGAL.md` (política de ROMs y marcas), `THIRD_PARTY_NOTICES.md` (atribución de bsnes, Snes9x, libretro shaders, SDL, etc.) y `CONTRIBUTING.md` con cláusula DCO (Developer Certificate of Origin) para cada commit.

## 3. Requisitos funcionales

La experiencia se organiza en tres niveles de complejidad que el usuario elige al primer arranque y puede cambiar en cualquier momento: **Simple** (solo jugar), **Avanzado** (toda la personalización) y **Desarrollador** (depuración y herramientas). Cada opción del emulador declara en qué nivel es visible.

| ID | Área | Requisito | Prioridad |
| --- | --- | --- | --- |
| F-01 | Biblioteca | Escaneo de carpetas, identificación por hash (CRC32/SHA-1 contra No-Intro), vista grilla/lista, filtros por región, género y estado de compatibilidad | P0 |
| F-02 | Biblioteca | Arte opcional (carátula, captura) desde libretro-thumbnails; caché local; modo sin conexión completo | P1 |
| F-03 | Carga | Arrastrar y soltar ROM; soporte .zip/.7z; detección automática de cabecera copier (512 bytes) y mapeo LoROM/HiROM/ExHiROM; detección de chips especiales | P0 |
| F-04 | Ejecución | Ciclo exacto a 60,0988 Hz (NTSC) / 50,007 Hz (PAL); sincronización de audio dinámica (ajuste de pitch ±0,5 %) para evitar tartamudeo | P0 |
| F-05 | Estados | 10 ranuras de guardado rápido + ilimitadas con nombre, miniatura y marca de tiempo; autoguardado al salir; compatibilidad con SRAM y RTC (S-RTC, SPC7110) | P0 |
| F-06 | Latencia | Run-ahead (1–3 frames) y preemptive frames; opción de VRR y vsync adaptativo; medición de latencia integrada en el HUD | P0 |
| F-07 | Tiempo | Rebobinado (buffer configurable 10–120 s), avance rápido con límite de fps, cámara lenta, avance frame a frame | P1 |
| F-08 | Entrada | Mapeo libre por dispositivo (SDL3 GameController), perfiles por juego, soporte de Mouse SNES, Super Scope, Justifier, Multitap (5 jugadores), turbo configurable, deadzone y curvas analógicas | P0 |
| F-09 | Personalización | Temas de interfaz (claro, oscuro, retro, personalizados en JSON), bezels/overlays, tipografía, tamaño de UI, idioma (es, en, pt, ja, de, fr) | P1 |
| F-10 | Personalización | Perfiles de configuración por juego con herencia (global → consola → juego); exportación e importación de perfiles | P1 |
| F-11 | Trucos | Game Genie / Pro Action Replay / códigos RAW; base de datos de trucos comunitaria abierta (libretro-database cht); editor manual | P2 |
| F-12 | Red | Netplay P2P con rollback (GGPO-like) para 2 jugadores; sala con código; relé opcional autohospedado | P2 |
| F-13 | Logros | Integración opcional con RetroAchievements (rcheevos) activada por el usuario | P2 |
| F-14 | Captura | Captura de pantalla PNG sin filtro / con filtro; grabación de video (FFmpeg, H.264/AV1) y audio WAV/FLAC; grabación de inputs (TAS/BK2-like) | P2 |
| F-15 | Accesibilidad | Navegación completa por teclado y mando; lector de pantalla en menús; alto contraste; remapeo de colores para daltonismo en la salida emulada | P1 |
| F-16 | Desarrollador | Depurador 65C816 y SPC700 (breakpoints, watchpoints, trazas), visor de VRAM/OAM/CGRAM, perfilador de frame, volcado de memoria, soporte de símbolos WLA-DX/ca65 | P2 |
| F-17 | Portabilidad | Instalación portable (configuración junto al ejecutable) o por usuario; actualizaciones in-app con firma y canal estable/beta | P1 |
| F-18 | Onboarding | Asistente de primer arranque: elegir nivel, carpeta de ROMs, mando, preset gráfico detectado según GPU; primer juego en < 60 s | P0 |

**Fuera de alcance v1.0:** emulación de Satellaview en línea, Game Boy vía Super Game Boy (planeado v1.2 mediante core SameBoy), realidad virtual, interfaz web.

## 4. Arquitectura técnica

La arquitectura es de cuatro capas con una regla no negociable: el núcleo no conoce ninguna API gráfica, de audio ni de ventana. Produce un framebuffer indexado (hasta 512×478 con colores de 15 bits y flags de interlace/hi-res) y muestras PCM a 32 040 Hz; todo lo demás ocurre en los backends.

&#91;embedded content: arquitectura por capas · frontend, API, núcleo, backends\]

El frontend habla solo con `libheno-core` a través de una API en C; los backends se seleccionan en tiempo de ejecución según la GPU y el sistema operativo detectados.

**Decisiones de diseño clave:**

1. **Núcleo dual.** Un único árbol de código con dos perfiles de compilación: `accuracy` (ciclo exacto, derivado del modelo de bsnes/higan, referencia para pruebas) y `performance` (ejecución por scanline con sincronización lazy, derivado de los principios de Snes9x, para hardware débil y Android). Ambos exponen la misma API y estados de guardado compatibles.
2. **Scheduler cooperativo.** Hilos lógicos (CPU, PPU, APU, coprocesadores) con relojes independientes sincronizados en puntos de acceso compartido; permite run-ahead sin duplicar el estado completo cada frame.
3. **Serialización determinista.** Todo el estado se serializa en un formato binario versionado con migraciones; es la base de estados, rebobinado, netplay rollback y pruebas de regresión por hash.
4. **API en C estable (`heno.h`).** Carga/descarga, paso de frame, callbacks de video/audio/input, serialización, acceso a memoria para trucos y depurador. Permite compilar un `libretro core` adaptador en fase 3 sin tocar el núcleo.
5. **Frontend Qt 6.** Qt Widgets para el shell principal (menús, biblioteca, ajustes) y QML para temas y transiciones; ImGui en overlay sobre la superficie de juego para HUD y herramientas de depuración de alta frecuencia.
6. **Plataforma vía SDL3.** Ventana, eventos, mandos, hilos de audio y detección de displays (VRR, HDR, refresco). Qt no toca el loop de emulación: la ventana de juego es una superficie nativa entregada al backend gráfico.
7. **Hilo de emulación aislado.** El núcleo corre en un hilo con prioridad alta; UI y emulación se comunican por colas lock-free; el backend de video presenta desde su propio hilo cuando la API lo permite (Vulkan/D3D12).

## 5. Pipeline de renderizado

El renderizado se diseña como una capa de abstracción propia (`heno-gfx`) con cuatro backends de igual prioridad, de modo que ninguna función de usuario dependa de una API concreta. La ruta moderna es la predeterminada en GPUs con soporte; la ruta OpenGL es la red de seguridad para hardware de 2010 en adelante.

| Backend | Plataformas | Mínimo | Capacidades exclusivas |
| --- | --- | --- | --- |
| Vulkan 1.3 | Windows, Linux, Android | GCN 1.0 (AMD), Kepler (NVIDIA), Gen9 (Intel) con drivers actuales | Dynamic rendering, timeline semaphores, presentación con hilo propio, VK\_KHR\_present\_wait para latencia mínima |
| Direct3D 12 | Windows 10/11 | FL 12\_0 | Flip model + DXGI waitable swapchain, HDR10/scRGB nativo, integración con Auto HDR y DirectX 12 Agility SDK |
| Metal 3 | macOS 12+, iOS futuro | Apple Silicon o AMD/Intel con Metal 3 | ProMotion adaptativo, EDR en pantallas XDR |
| OpenGL 3.3 / GLES 3.0 | Todas (legado) | Cualquier GPU 2010+ | Ruta de compatibilidad; sin HDR ni presentación asíncrona |

**Shaders.** Un único lenguaje fuente: GLSL 450 con la extensión de presets `.slangp` de libretro (ya existen más de 400 shaders GPL: CRT-Royale, CRT-Guest-Advanced, xBRZ, ScaleFX, NTSC-adaptive). El proyecto compila a SPIR-V con `glslang` y traduce a HLSL/MSL/GLSL mediante `SPIRV-Cross` en tiempo de carga, con caché de pipelines en disco por GPU y versión de driver. El editor de shaders permite ajustar parámetros en vivo y encadenar hasta 16 pasadas con texturas de historial (frame anterior) para efectos de fósforo y motion blur CRT.

**Escalado y filtros nativos (sin shader):** entero (1×–8×), entero adaptativo (máximo que cabe), relación de aspecto 4:3 / 8:7 / PAR exacto 1,143 / estirado, sharp-bilinear, rotación para pantallas verticales.

**Tecnologías modernas por fabricante** (todas opcionales, detectadas en arranque):

| Capacidad | AMD | NVIDIA | Intel / otros |
| --- | --- | --- | --- |
| VRR | FreeSync / Adaptive-Sync vía VK\_EXT\_present\_timing y DXGI | G-Sync / G-Sync Compatible | Adaptive-Sync |
| Baja latencia | Anti-Lag 2 (SDK público) | Reflex (NVAPI; carga dinámica, no es dependencia dura) | Marcador de frame genérico |
| HDR | scRGB/HDR10 vía DXGI y VK\_EXT\_hdr\_metadata | Igual + RTX HDR como opción de driver | Igual |
| Upscaling con IA | Ninguno oficial; soporte de FSR 1 (espacial, GPL) para usuarios que prefieran suavizado | Sin DLSS: la salida 256×224 no es apta para super-resolución temporal | — |
| Captura | — | — | Captura por FFmpeg con aceleración VAAPI/NVENC/AMF |

**Latencia como requisito de primera clase.** Objetivo: entrada a fotón ≤ 1,5 frames. Mecanismos: run-ahead (emula N frames extra y descarta), swapchain con 1 frame en vuelo, presentación justo antes del vblank (frame pacing con `present_wait`/waitable object), opción de deshabilitar la composición en pantalla completa exclusiva, y HUD con medición real mediante marcas de tiempo de GPU.

**Presets de calidad detectados en arranque:** `Legado` (OpenGL, sin shader), `Equilibrado` (Vulkan/D3D12, CRT ligero, entero), `Máximo` (CRT-Royale, HDR, VRR, run-ahead 2). El usuario puede cambiar y guardar presets por juego.

## 6. Stack tecnológico y estructura del repositorio

Todas las dependencias son open source con licencias compatibles con GPLv3 y se gestionan con vcpkg en modo manifest para reproducibilidad entre plataformas.

| Componente | Tecnología | Versión mínima | Rol |
| --- | --- | --- | --- |
| Lenguaje | C++20 (Clang 17 / GCC 13 / MSVC 19.38) | — | Núcleo, backends, frontend |
| Build | CMake 3.28 + Ninja + vcpkg | — | Multiplataforma, presets por SO |
| UI | Qt 6.7 (Widgets + Quick) | 6.7 LTS | Shell principal, temas |
| Overlay | Dear ImGui | 1.90 | HUD, depurador |
| Plataforma | SDL3 | 3.2 | Ventana, input, audio, displays |
| Gráficos | Vulkan SDK, D3D12 Agility SDK, Metal-cpp, glad | — | Backends |
| Shaders | glslang, SPIRV-Cross, slang-shaders (libretro) | — | Compilación y presets |
| Audio DSP | libsamplerate (sinc) + resampler propio | — | Resample 32 kHz → dispositivo |
| Compresión | libarchive, zstd | — | .zip/.7z, estados comprimidos |
| Red | ENet + ggpo-like propio (`heno-rollback`) | — | Netplay |
| Logros | rcheevos | — | RetroAchievements |
| Video | FFmpeg (dlopen, no enlazado) | 6.x | Grabación |
| Pruebas | Catch2, hash de frames de referencia, Snes9x/bsnes test suites | — | Regresión |
| CI | GitHub Actions (Windows, Ubuntu, macOS arm64/x86\_64), ccache, sccache | — | Builds nocturnas y release |

```markdown
henosnes/
├── CMakeLists.txt · CMakePresets.json · vcpkg.json
├── LICENSE · LEGAL.md · THIRD_PARTY_NOTICES.md · CONTRIBUTING.md · SECURITY.md
├── docs/            # ADRs (decisiones de arquitectura), guía de usuario, API
├── core/            # libheno-core (sin dependencias de UI ni SO)
│   ├── cpu/         # 65C816, scheduler
│   ├── ppu/         # S-PPU1/2, modos 0-7, hi-res, interlace
│   ├── apu/         # SPC700, S-DSP
│   ├── cart/        # mapeo, SRAM, RTC, coprocesadores HLE/LLE
│   ├── serial/      # estados versionados
│   └── api/         # heno.h (C ABI)
├── gfx/             # heno-gfx: abstracción + backends vulkan/ d3d12/ metal/ opengl/
├── shaders/         # submódulo slang-shaders + presets propios
├── platform/        # SDL3: ventana, audio, input, displays
├── frontend/        # Qt 6: biblioteca, ajustes, temas, i18n
├── overlay/         # ImGui: HUD, depurador, perfilador
├── net/             # netplay rollback, relé opcional
├── tools/           # heno-cli (headless), rom-info, state-inspect
├── tests/           # unit, integration, roms homebrew libres, golden frames
├── packaging/       # MSIX/NSIS, AppImage/Flatpak, .dmg notarizado
└── .github/workflows/
```

**Convenciones para el agente de desarrollo:** un ADR en `docs/adr/NNNN-titulo.md` por cada decisión no trivial; clang-format y clang-tidy obligatorios en CI; sin excepciones C++ en `core/` (códigos de retorno + `std::expected`); todo acceso a memoria emulada pasa por el bus con trazabilidad para el depurador; cobertura mínima del 80 % en `core/serial/` y `core/cart/`.

## 7. Roadmap por fases y criterios de aceptación

Seis fases secuenciales, cada una con un gate verificable por CI antes de abrir la siguiente. Las duraciones asumen un equipo pequeño apoyado por agentes y son estimaciones a validar en la fase 0.

1. **Fase 0 — Cimientos (2 semanas).** Repositorio, CMake multiplataforma, vcpkg, CI en tres SO, `heno.h` vacío con pruebas de ABI, ADR-0001 (licencia y alcance legal), ADR-0002 (arquitectura por capas).
   - Gate: build verde en Windows/Linux/macOS; `heno-cli --version` empaquetado en los tres.
2. **Fase 1 — Núcleo mínimo (8 semanas).** CPU 65C816 completa, PPU modos 0–1, APU, mapeo LoROM/HiROM, SRAM, estados, salida por framebuffer a `heno-cli` headless que vuelca PNG.
   - Gate: 100 % de los test ROMs de CPU/SPC700 de la comunidad en verde; 20 ROMs homebrew libres arrancan y generan frames idénticos a la referencia (hash).
3. **Fase 2 — Núcleo completo + OpenGL (8 semanas).** PPU modos 2–7, hi-res, interlace, Mouse/Super Scope/Multitap, coprocesadores SA-1, SuperFX, DSP-1 (HLE), S-DD1, SPC7110. Backend OpenGL 3.3 y ventana SDL3 con audio sincronizado.
   - Gate: ≥ 97 % del set homebrew sin fallos gráficos conocidos; 60 fps en Intel UHD 620 sin shaders; audio sin cortes 10 minutos continuos.
4. **Fase 3 — Frontend y personalización (8 semanas).** Shell Qt 6, biblioteca con hash No-Intro, asistente de primer arranque, perfiles por juego, mapeo de mandos, temas, i18n es/en, run-ahead, rebobinado, HUD ImGui.
   - Gate: prueba de usabilidad con 5 personas sin experiencia: primer juego en < 60 s en 5 de 5; auditoría de accesibilidad con teclado completa.
5. **Fase 4 — Renderizado moderno (10 semanas).** Backends Vulkan, D3D12 y Metal; pipeline slang con SPIRV-Cross y caché; HDR, VRR, frame pacing con present\_wait; presets por GPU; integración opcional Reflex/Anti-Lag 2.
   - Gate: paridad visual entre backends (diferencia de imagen < 0,5 % con CRT-Royale); latencia medida ≤ 1,5 frames en AMD RX 6600 y NVIDIA RTX 3060; sin crash en 24 h de ciclo de shaders aleatorio.
6. **Fase 5 — Comunidad y 1.0 (6 semanas).** Trucos, RetroAchievements, grabación, netplay rollback 2 jugadores, depurador, documentación de usuario, empaquetado firmado (MSIX, Flatpak/AppImage, .dmg notarizado), canal de actualizaciones.
   - Gate: 0 issues bloqueantes abiertos; beta pública de 4 semanas con ≥ 200 usuarios; publicación en Flathub y winget.

**Post-1.0 (v1.1–1.3):** Android y Steam Deck (UI táctil/gamepad-first), core libretro, Super Game Boy mediante SameBoy, netplay 4 jugadores, editor visual de shaders.

## 8. Plan de ejecución en Antigravity

**Modelo recomendado: Claude Opus 4.6 (Thinking) para planificación y núcleo; Gemini 3.1 Pro como segundo agente de revisión; Gemini Flash (3.7/3.8) para ediciones rápidas y scaffolding.** Según la lista oficial de Antigravity a septiembre de 2026, el plan Individual incluye Gemini 3.8/3.7/3.6 Flash, Gemini 3.1 Pro, Claude Sonnet y Opus 4.6 y GPT-OSS-120b ([fuente](https://www.glbgpt.com/hub/es/what-is-google-antigravity/)). Los modelos no-Google tienen una cuota separada y más baja ([fuente](https://100x.mx/blog/que-es-google-antigravity-guia-principiantes/)), por lo que conviene reservar Opus para las tareas donde la precisión ciclo-exacta y el razonamiento largo son críticos (CPU, PPU, scheduler, serialización) y delegar UI, empaquetado y documentación a Gemini.

| Tipo de tarea | Agente / modelo | Modo Antigravity | Motivo |
| --- | --- | --- | --- |
| Arquitectura, ADRs, API en C, scheduler, CPU/PPU/APU | Claude Opus 4.6 (Thinking) | Planning | Razonamiento largo sobre hardware documentado; menor tasa de errores sutiles de timing |
| Backends gráficos (Vulkan/D3D12/Metal) | Claude Opus 4.6 → revisión Gemini 3.1 Pro | Planning + Fast | Dos modelos distintos detectan más errores de sincronización GPU |
| Frontend Qt, temas, i18n, biblioteca | Gemini 3.1 Pro | Fast | Alto volumen de código repetitivo con bajo riesgo |
| Pruebas, golden frames, CI, empaquetado | Gemini Flash | Fast | Tareas mecánicas, cuota abundante |
| Revisión legal de dependencias y licencias | Claude Sonnet 4.6 | Planning | Lectura cuidadosa de textos de licencia |
| Verificación en navegador (docs, Flathub, winget) | Agente con control de navegador (cualquier modelo) | Fast | Validación end-to-end |

**Archivos de gobierno del agente** (en la raíz del repo, leídos por Antigravity en cada sesión):

- `AGENTS.md`: resumen de esta SPEC, reglas duras legales (sección 2), convenciones (sección 6), definición de terminado por fase (sección 7).
- `docs/adr/`: cada agente debe crear o actualizar un ADR antes de cambiar una decisión arquitectónica.
- `.antigravity/playbooks/`: playbooks reutilizables (`add-coprocessor`, `add-gfx-backend`, `add-shader-preset`, `release`).

**Reglas duras para todos los agentes:**

1. Nunca descargar, buscar ni referenciar ROMs comerciales; solo las homebrew con licencia libre listadas en `tests/roms/MANIFEST.json`.
2. Nunca copiar código de repositorios con licencia incompatible con GPLv3; registrar origen y licencia de cada archivo portado en `THIRD_PARTY_NOTICES.md`.
3. Cada PR del núcleo debe pasar la suite de golden frames; una regresión bloquea el merge aunque el agente la considere "mejora".
4. Sin dependencias nuevas sin ADR aprobado por un humano.
5. Un PR = una tarea del backlog; tamaño máximo 800 líneas netas salvo generación de tablas.

**Secuencia de prompts (uno por sesión de Agent Manager, en modo Planning salvo indicación):**

```markdown
P-00 · Bootstrap
Lee SPEC.md y AGENTS.md. Crea la estructura de carpetas de la sección 6, CMakePresets
para windows-msvc, linux-clang, macos-arm64 y macos-x64, vcpkg.json con las dependencias
de la tabla, GitHub Actions que compile heno-cli en los tres SO y suba artefactos.
Escribe ADR-0001 y ADR-0002. Gate: CI verde. No implementes emulación todavía.

P-01 · CPU 65C816
Implementa core/cpu con todas las instrucciones y modos de direccionamiento del 65C816,
con conteo de ciclos por acceso al bus. Usa la documentación de referencia en docs/refs.
Agrega pruebas unitarias por opcode y ejecuta los test ROMs de CPU de tests/roms.
Gate: 100 % en verde. Registra en docs/adr/0003 la estrategia de scheduler.

P-02 · PPU modos 0-1 y framebuffer headless
...

P-05 · Backend OpenGL + SDL3 (modo Fast tras plan aprobado)
...

P-09 · Backend Vulkan con present_wait y caché de pipelines
Implementa gfx/vulkan siguiendo la interfaz de gfx/igfx.h. Prioriza latencia: 1 frame en
vuelo, timeline semaphores, VK_KHR_present_wait cuando exista. Prueba en software con
lavapipe en CI. Mide paridad con OpenGL mediante tests/golden. Gate: diferencia < 0,5 %.
```

**Flujo por sesión:** (1) el agente lee `AGENTS.md` y el backlog en `docs/BACKLOG.md`; (2) genera un plan y lo guarda como artefacto para revisión humana; (3) implementa en rama `feat/<id>`; (4) ejecuta pruebas locales y CI; (5) abre PR con enlace al ADR y a la evidencia (hashes de golden frames, capturas); (6) un segundo agente con modelo distinto revisa; (7) un humano aprueba el merge.

## 9. Pruebas, CI/CD, licencias y gobernanza

La calidad se garantiza por regresión automática: cada frame de referencia es un hash reproducible y cada backend gráfico debe producir el mismo resultado dentro de una tolerancia conocida.

| Nivel | Qué prueba | Herramienta | Frecuencia |
| --- | --- | --- | --- |
| Unitario | Opcodes, DSP, mapeo, serialización ida y vuelta | Catch2 | Cada PR |
| Test ROMs | CPU 65C816, SPC700, PPU timing (suites comunitarias libres) | heno-cli headless | Cada PR |
| Golden frames | 50+ ROMs homebrew libres, 300 frames cada una, hash SHA-256 por frame | heno-cli + tests/golden | Cada PR |
| Paridad de backends | Misma escena en OpenGL, Vulkan (lavapipe), D3D12 (WARP), Metal | Comparación de imagen (SSIM ≥ 0,995) | Nocturna |
| Rendimiento | fps y latencia en perfiles de referencia; alerta si cae > 5 % | Benchmarks propios + gráficas en CI | Nocturna |
| Fuzzing | Cargador de ROM, parser de estados, parser de presets slang | libFuzzer / OSS-Fuzz | Semanal |
| Sanitizers | ASan, UBSan, TSan en núcleo y colas lock-free | Clang | Nocturna |
| Estática | clang-tidy, cppcheck, escaneo de licencias (REUSE, scancode) | CI | Cada PR |

**Release.** Versionado semántico; canales `stable` y `beta`; builds reproducibles con fecha fija; firmas: Authenticode (Windows), notarización (macOS), GPG + Flathub (Linux). Changelog generado desde PRs con etiquetas convencionales. Actualizador in-app verifica firma antes de aplicar.

**Licencias y cumplimiento.** GPL-3.0-or-later para el proyecto; cumplimiento REUSE 3.0 (cabecera SPDX en cada archivo); `THIRD_PARTY_NOTICES.md` generado automáticamente desde vcpkg y submódulos; política de contribución con DCO; revisión manual de cualquier código portado desde bsnes, Snes9x o Mesen antes del merge.

**Gobernanza.** Mantenedor principal + 2 mantenedores de área (núcleo, gráficos); decisiones por ADR y votación simple en issues etiquetadas `decision`; Código de Conducta (Contributor Covenant 2.1); `SECURITY.md` con divulgación responsable; reuniones públicas mensuales con acta en `docs/meetings/`; roadmap público como GitHub Project.

**Riesgos principales y mitigación:**

| Riesgo | Impacto | Mitigación |
| --- | --- | --- |
| Errores sutiles de timing generados por IA | Juegos rotos difíciles de detectar | Golden frames + test ROMs obligatorios; revisión cruzada con segundo modelo |
| Deuda de licencias por código portado | Bloqueo legal del proyecto | Escaneo automático + registro de origen por archivo |
| Divergencia entre backends gráficos | Experiencia inconsistente AMD/NVIDIA/Apple | Pruebas de paridad nocturnas; abstracción única `heno-gfx` |
| Cuota de modelos en Antigravity | Ralentización | Asignación por tipo de tarea (sección 8); Opus solo en núcleo |
| Reclamos de marca | Exposición legal | Nombre propio, uso descriptivo, sin logos de terceros |

**Fuentes consultadas:** [glbgpt — modelos en Antigravity, sept. 2026](https://www.glbgpt.com/hub/es/what-is-google-antigravity/) · [100x — cuotas de modelos no-Google](https://100x.mx/blog/que-es-google-antigravity-guia-principiantes/). Las especificaciones de hardware SNES y las características de APIs gráficas provienen de conocimiento técnico general y deben verificarse contra la documentación oficial durante la fase 0.
