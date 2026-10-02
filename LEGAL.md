<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# Legal — henoSNES

## What this project does

- Emulates SNES hardware (CPU 65C816, PPU, APU, coprocessors) using original
  implementations or GPL-compatible third-party code with proper attribution.
- Loads ROM files that the user already possesses on their local storage
  (formats: .sfc, .smc, .fig, .swc, .bs, compressed in .zip/.7z).
- Includes, for testing and demonstration only, homebrew ROMs with explicit
  libre licenses listed in `tests/roms/MANIFEST.json`.

## What this project does NOT do

- **No commercial ROM sources.** The project never downloads, links, indexes,
  or suggests sources for commercial ROMs. There is no "search ROM online"
  feature or scraper.
- **No proprietary BIOS/firmware.** The SNES does not require a BIOS to boot.
  Coprocessors (DSP-1..4, ST010/011, Cx4) use HLE reimplementations. Users
  may optionally provide firmware files for LLE.
- **No incompatible code.** The project never incorporates code from projects
  with licenses incompatible with GPL-3.0, nor leaked/stolen code from
  Nintendo or any other party.
- **No trademark infringement.** The name "henoSNES" uses "SNES" purely as a
  descriptive term indicating compatibility. The project does not reproduce
  any logos, typefaces, or visual designs belonging to Nintendo.

## Trademarks

"Super Nintendo", "Super Famicom", "SNES", and related marks are trademarks
of Nintendo Co., Ltd. henoSNES is not affiliated with, endorsed by, or
sponsored by Nintendo. The name "henoSNES" uses "SNES" solely in a
descriptive capacity to indicate the type of hardware being emulated.

## Third-party code

All third-party code is listed in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)
with source URL, commit hash, and license identifier.
