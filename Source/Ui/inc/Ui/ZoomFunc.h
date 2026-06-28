#pragma once

#include <Platform/NumTypes.h>

#include <algorithm>
#include <cmath>
#include <concepts>

namespace Ui::Zoom {
    // Continuous exponential zoom: factor = Multiplier^level.
    // Negative levels zoom out, positive zoom in, level 0 -> factor 1.
    // Levels are clamped to [MinLevel, MaxLevel] inside the function so Panel can detect
    // a no-op increment and not drift past the bound.
    // Exponential<f32, 1.1f>           -> 10% per step, -10..+10 range.
    // Exponential<f32, 1.25f, -4, 4>   -> 25% per step, -4..+4 range.
    template<std::floating_point T, T Multiplier, s32 MinLevel = -10, s32 MaxLevel = 10>
    T Exponential(s32 level) {
        return static_cast<T>(std::pow(Multiplier, std::clamp(level, MinLevel, MaxLevel)));
    }

    // Discrete stages centered on level 0. Index of level 0 is (count - 1) / 2.
    // Negative levels shift toward values[0] (zoom out), positive toward values[count - 1] (zoom in).
    // Both ends clamp.
    // Discrete<f32, 0.5f, 1.0f, 2.0f> -> level -1 -> 0.5, level 0 -> 1.0, level +1 -> 2.0.
    template<std::floating_point T, T... Values>
    constexpr T Discrete(s32 level) {
        static_assert(sizeof...(Values) > 0, "Ui::Zoom::Discrete requires at least one value");
        static constexpr T values[] = {Values...};
        static constexpr s32 count = static_cast<s32>(sizeof...(Values));
        static constexpr s32 center = (count - 1) / 2;
        const s32 idx = center + level;
        if(idx < 0) return values[0];
        if(idx >= count) return values[count - 1];
        return values[idx];
    }
} // namespace Ui::Zoom
