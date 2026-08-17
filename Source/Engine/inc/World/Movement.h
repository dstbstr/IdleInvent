#pragma once

#include "World/World.h"

namespace World {
    template<size_t TWidth, size_t THeight>
    constexpr WorldLocation Offset(WorldLocation from, Displacement offset) {
        auto width = static_cast<f32>(TWidth);
        auto height = static_cast<f32>(THeight);
        auto result = from;
        result.Local.X += offset.X;
        result.Local.Y += offset.Y;

        while(result.Local.X < 0.0f) {
            result.Local.X += width;
            result.Chunk.X--;
        }
        while(result.Local.X >= width) {
            result.Local.X -= width;
            result.Chunk.X++;
        }
        while(result.Local.Y < 0.0f) {
            result.Local.Y += height;
            result.Chunk.Y--;
        }
        while(result.Local.Y >= height) {
            result.Local.Y -= height;
            result.Chunk.Y++;
        }

        return result;
    }

    template<size_t TWidth, size_t THeight, typename TCanOccupy>
    constexpr bool TryMove(WorldLocation& from, Displacement offset, const TCanOccupy& CanOccupy) {
        auto candidate = Offset<TWidth, THeight>(from, offset);
        if(CanOccupy(candidate)) {
            from = candidate;
            return true;
        }

        return false;
    }
}