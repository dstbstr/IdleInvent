#pragma once
#include <Platform/NumTypes.h>

#include <limits>
#include <cmath>

namespace Smoothstep {
	constexpr f64 Cubic(f64 t) { return t * t * (3.0 - 2.0 * t); }
	constexpr f64 Quintic(f64 t) { return t * t * t * (t * (t * 6.0 - 15.0) + 10.0); }
}

namespace Noise {
    constexpr u32 MixSeed(u32 value) {
        value ^= value >> 16;
        value *= 0x7f3b352dU;
        value ^= value >> 15;
        value *= 0x846ca68bU;
        value ^= value >> 16;
        return value;
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr f64 Get2D(u32 seed, f64 x, f64 y, auto Smooth) {
        auto x0 = static_cast<s64>(std::floor(x));
        auto y0 = static_cast<s64>(std::floor(y));
        auto x1 = x0 + 1;
        auto y1 = y0 + 1;

        auto tx = x - static_cast<f64>(x0);
        auto ty = y - static_cast<f64>(y0);
        auto smoothX = Smooth(tx);
        auto smoothY = Smooth(ty);

        auto LatticeValue = [seed](s64 x, s64 y) { 
            auto mixed = MixSeed(seed);
            mixed ^= MixSeed(static_cast<u32>(x));
            mixed = MixSeed(mixed);
            mixed ^= MixSeed(static_cast<u32>(y));
            return static_cast<f64>(MixSeed(mixed)) / static_cast<f64>(std::numeric_limits<u32>::max());
        };

        auto v00 = LatticeValue(x0, y0);
        auto v10 = LatticeValue(x1, y0);
        auto v01 = LatticeValue(x0, y1);
        auto v11 = LatticeValue(x1, y1);

        auto top = std::lerp(v00, v10, smoothX);
        auto bottom = std::lerp(v01, v11, smoothX);
        return std::lerp(top, bottom, smoothY);
    }

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    constexpr f64 Fractal2D(u32 seed, f64 x, f64 y, size_t octaves, f64 baseFrequency, auto Smooth) {
        if(octaves == 0) return 0.0;

        auto value = 0.0;
        auto amplitude = 1.0;
        auto frequency = baseFrequency;
        auto totalAmplitude = 0.0;
        for(size_t i = 0; i < octaves; ++i) {
            value += Get2D(seed + static_cast<u32>(i), x * frequency, y * frequency, Smooth) * amplitude;
            totalAmplitude += amplitude;
            amplitude *= 0.5;
            frequency *= 2.0;
        }

        return value / totalAmplitude;
    }
}