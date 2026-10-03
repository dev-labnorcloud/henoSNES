// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#ifdef _MSC_VER
#include <crtdbg.h>
#include <stdlib.h>
#endif

#include "sched/scheduler.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

using namespace heno;

extern "C" void on_abort(int) {
    std::_Exit(EXIT_FAILURE);
}

struct MockApu {
    void catch_up(std::uint64_t target) {}
};

struct MockPpu {
    void* scheduler = nullptr;
    void catch_up(std::uint64_t target) {
        if (scheduler) {
            reinterpret_cast<Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu>*>(scheduler)->advance(10);
        }
    }
};

int main() {
#ifdef _MSC_VER
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
#endif

    std::signal(SIGABRT, on_abort);

    MockPpu ppu;
    MockApu apu;
    Scheduler<SyncPolicy::Accuracy, MockPpu, MockApu> sched(ppu, apu, kApuRatioNtsc);
    ppu.scheduler = &sched;

    sched.advance(10);

    std::cerr << "reentrancia no detectada\n";
    return 0; // Return 0 means success, which makes CTest fail because of WILL_FAIL TRUE
}
