<!-- SPDX-FileCopyrightText: 2026 henoSNES contributors -->
<!-- SPDX-License-Identifier: GPL-3.0-or-later -->

# henoSNES

Open-source SNES / Super Famicom emulator. GPL-3.0-or-later. Multiplataforma
(Windows, Linux, macOS). C++20 core with cycle-accurate emulation and modern
rendering backends (Vulkan 1.3, Direct3D 12, Metal 3, OpenGL 3.3).

## Status

**Phase 1 — Minimal core (in progress).** The cooperative scheduler (P1-005) and the memory bus with LoROM/HiROM mapping (P1-006) are implemented and tested. The CPU, PPU and APU are not implemented yet, so no game runs.

## Build

```bash
# Configure (pick your platform preset)
cmake --preset linux-clang-release   # or windows-msvc, macos-arm64-release, macos-x64-release

# Build
cmake --build --preset linux-clang-release

# Test
ctest --preset linux-clang-release --output-on-failure

# Run
./build/linux-clang-release/tools/heno-cli/heno-cli --version
```

Requires CMake 3.28+, Ninja, vcpkg, and a C++20 compiler (Clang 17 / GCC 13 / MSVC 19.38).

## License

GPL-3.0-or-later. See [LICENSE](LICENSE) and [LEGAL.md](LEGAL.md).

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines and DCO requirements.

## Security

See [SECURITY.md](SECURITY.md) for vulnerability reporting.
