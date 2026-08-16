#pragma once

#include "World/World.h"

namespace World {
    template<size_t TWidth, size_t THeight>
    constexpr WorldLocation Offset(WorldLocation from, Displacement offset) {
        auto width = static_cast<f32>(TWidth);
        auto height = static_cast<f32>(THeight);
        auto result = from;
        result.Pos.X += offset.X;
        result.Pos.Y += offset.Y;

        while(result.Pos.X < 0.0f) {
            result.Pos.X += width;
            result.ChunkCoord.X--;
        }
        while(result.Pos.X >= width) {
            result.Pos.X -= width;
            result.ChunkCoord.X++;
        }
        while(result.Pos.Y < 0.0f) {
            result.Pos.Y += height;
            result.ChunkCoord.Y--;
        }
        while(result.Pos.Y >= height) {
            result.Pos.Y -= height;
            result.ChunkCoord.Y++;
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