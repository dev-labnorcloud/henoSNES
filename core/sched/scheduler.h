// SPDX-FileCopyrightText: 2026 henoSNES contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "common/assert.h"

#include <concepts>
#include <cstdint>
#include <type_traits>

namespace heno {

enum class SyncPolicy { Accuracy, Performance };

struct ClockRatio {
    std::uint64_t num;
    std::uint64_t den;
};

// PAL ratio is omitted until verified.
inline constexpr ClockRatio kApuRatioNtsc{125312, 109375};

template <typename T>
concept Subordinate = requires(T& t, std::uint64_t target) {
    { t.catch_up(target) } -> std::same_as<void>;
};

struct SchedulerState {
    std::uint64_t master_cycle;
    std::uint64_t apu_cycle;
    std::uint64_t apu_acc;
};
static_assert(std::is_trivially_copyable_v<SchedulerState>);

template <SyncPolicy Policy, Subordinate Ppu, Subordinate Apu>
class Scheduler {
public:
    Scheduler(Ppu& ppu, Apu& apu, ClockRatio apu_ratio) noexcept
        : ppu_(ppu), apu_(apu), apu_ratio_(apu_ratio) {
        HENO_ASSERT(apu_ratio_.den != 0);
    }

    void advance(std::uint32_t master_cycles) noexcept {
        HENO_ASSERT(!in_call_);
        in_call_ = true;

        state_.master_cycle += master_cycles;
        state_.apu_acc += static_cast<std::uint64_t>(master_cycles) * apu_ratio_.num;
        state_.apu_cycle += state_.apu_acc / apu_ratio_.den;
        state_.apu_acc %= apu_ratio_.den;

        if constexpr (Policy == SyncPolicy::Accuracy) {
            catch_up_ppu();
            catch_up_apu();
        }

        in_call_ = false;
    }

    void sync_ppu() noexcept {
        HENO_ASSERT(!in_call_);
        in_call_ = true;
        catch_up_ppu();
        in_call_ = false;
    }

    void sync_apu() noexcept {
        HENO_ASSERT(!in_call_);
        in_call_ = true;
        catch_up_apu();
        in_call_ = false;
    }

    void sync_all() noexcept {
        HENO_ASSERT(!in_call_);
        in_call_ = true;
        catch_up_ppu();
        catch_up_apu();
        in_call_ = false;
    }

    const SchedulerState& state() const noexcept {
        return state_;
    }

    void load_state(const SchedulerState& new_state) noexcept {
        HENO_ASSERT(!in_call_);
        state_ = new_state;
        ppu_synced_until_ = state_.master_cycle;
        apu_synced_until_ = state_.apu_cycle;
    }

private:
    void catch_up_ppu() noexcept {
        if (ppu_synced_until_ < state_.master_cycle) {
            ppu_.catch_up(state_.master_cycle);
            ppu_synced_until_ = state_.master_cycle;
        }
    }

    void catch_up_apu() noexcept {
        if (apu_synced_until_ < state_.apu_cycle) {
            apu_.catch_up(state_.apu_cycle);
            apu_synced_until_ = state_.apu_cycle;
        }
    }

    Ppu& ppu_;
    Apu& apu_;
    ClockRatio apu_ratio_;
    SchedulerState state_{0, 0, 0};

    std::uint64_t ppu_synced_until_{0};
    std::uint64_t apu_synced_until_{0};
    bool in_call_{false};
};

} // namespace heno
