// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstdio>
#include <cstring>
#include "heno.h"

#ifndef HENO_VERSION
#define HENO_VERSION "0.0.1"
#endif

#ifndef HENO_GIT_HASH
#define HENO_GIT_HASH "unknown"
#endif

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        if (std::strcmp(argv[1], "--version") == 0 || std::strcmp(argv[1], "-v") == 0) {
            std::printf("henoSNES v%s (%s)\n", HENO_VERSION, HENO_GIT_HASH);
            return 0;
        }
        if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
            std::printf("Usage: heno-cli [options]\n");
            std::printf("Options:\n");
            std::printf("  -v, --version    Show version info\n");
            std::printf("  -h, --help       Show this help\n");
            std::printf("API version: %u\n", heno_api_version());
            return 0;
        }
    }

    std::printf("Usage: heno-cli [options]\n");
    std::printf("Options:\n");
    std::printf("  -v, --version    Show version info\n");
    std::printf("  -h, --help       Show this help\n");
    return 1;
}
