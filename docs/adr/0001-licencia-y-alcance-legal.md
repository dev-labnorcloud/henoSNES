<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# ADR-0001: Licencia y alcance legal

## Estado

Aceptado

## Fecha

2026-10-02

## Contexto

henoSNES es un emulador de Super Nintendo. Los emuladores operan en un área legal sensible:
se emula hardware de terceros y se interactúa con software protegido por derechos de autor.
El proyecto necesita una licencia que permita reutilizar código de emuladores existentes de
referencia (bsnes/higan bajo GPLv3, Snes9x bajo licencia permisiva con restricciones comerciales
en versiones antiguas, shaders de libretro bajo GPL) y que proteja las contribuciones.

La SNES no requiere BIOS propietaria para arrancar, lo cual elimina el principal riesgo legal
que afecta a emuladores de otros sistemas (PlayStation, Game Boy Advance, etc.).

## Decisión

1. **Licencia del proyecto:** GPL-3.0-or-later.
   - Compatible con bsnes/higan (GPLv3) y libretro shaders (GPL).
   - Obliga a que cualquier fork mantenga el código abierto.

2. **Política de ROMs:**
   - El proyecto NUNCA descarga, enlaza, indexa ni sugiere fuentes de ROMs comerciales.
   - Solo se incluyen ROMs homebrew con licencia libre explícita, listadas en
     `tests/roms/MANIFEST.json`.
   - Los usuarios cargan sus propias ROMs desde disco local.

3. **Política de BIOS/firmware:**
   - No se incluye BIOS, firmware ni microcódigo propietario.
   - Los coprocesadores (DSP-1..4, ST010/011, Cx4) se implementan en HLE.
   - El usuario puede proveer archivos de firmware para LLE si lo prefiere.

4. **Código de terceros:**
   - Nunca se copia código con licencia incompatible con GPLv3.
   - Nunca se usa código filtrado de Nintendo.
   - Todo archivo portado se registra en `THIRD_PARTY_NOTICES.md` con URL, commit y licencia.

5. **Marcas:**
   - El nombre "henoSNES" usa "SNES" de forma descriptiva.
   - No se reproducen logotipos, tipografías ni diseños de Nintendo.

## Consecuencias

- ✅ Compatibilidad con la base de código más precisa (bsnes) y el ecosistema de shaders más grande (libretro).
- ✅ Protección legal clara para contribuidores.
- ✅ Sin riesgo de BIOS propietaria (la SNES no la requiere).
- ⚠️ La GPL impide incorporar código de proyectos con licencias incompatibles (MIT con restricciones adicionales, etc.).
- ⚠️ Se necesita vigilancia continua al portar código: cada archivo requiere revisión manual de licencia.

## Documentos relacionados

- `LICENSE` — Texto de GPL-3.0-or-later
- `LEGAL.md` — Política de ROMs y marcas
- `THIRD_PARTY_NOTICES.md` — Atribución de código de terceros
- `CONTRIBUTING.md` — Cláusula DCO
