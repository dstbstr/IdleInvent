#pragma once
#include "Constexpr/ConstexprMath.h"

#include <Platform/NumTypes.h>
#include <array>

namespace World {
    struct Coord {
        s32 X, Y;
        constexpr auto operator<=>(const Coord&) const = default;
        constexpr Coord operator+(Coord delta) const { return {X + delta.X, Y + delta.Y}; }
    };
	struct LocalPos {
        f32 X, Y;
        constexpr auto operator<=>(const LocalPos&) const = default;
        constexpr LocalPos operator+(LocalPos delta) const { return {X + delta.X, Y + delta.Y}; }
    };
    using Displacement = LocalPos;

    struct WorldLocation {
        Coord ChunkCoord{};
        LocalPos Pos{};
        constexpr auto operator<=>(const WorldLocation&) const = default;
    };

	struct SquareTopology {
        constexpr std::array<Coord, 4> GetNeighbors(Coord pos) const {
            return {
                Coord{pos.X - 1, pos.Y},
                Coord{pos.X + 1, pos.Y},
                Coord{pos.X, pos.Y - 1},
                Coord{pos.X, pos.Y + 1}
            };
        }

        [[nodiscard]] constexpr u32 MinDistance(Coord from, Coord to) const {
            // yeah, yeah, overflow is possible, fix it if it becomes a problem
            return static_cast<u32>(Constexpr::Abs(from.X - to.X) + Constexpr::Abs(from.Y - to.Y));
        }

    	[[nodiscard]] constexpr bool IsNeighbor(Coord a, Coord b) const {
            return MinDistance(a, b) == 1;
        }
    };

    template<typename TCell, size_t TWidth, size_t THeight>
    struct Chunk {
        static_assert(TWidth > 0 && THeight > 0, "Chunks must have positive dimensions");
        static constexpr size_t Width{TWidth};
        static constexpr size_t Height{THeight};

        std::array<std::array<TCell, TWidth>, THeight> Cells{};

        [[nodiscard]] static constexpr bool Contains(Coord pos) {
            return pos.X >= 0 &&
                   pos.X < static_cast<s32>(Width) &&
                   pos.Y >= 0 &&
                   pos.Y < static_cast<s32>(Height);
        }

        [[nodiscard]] constexpr TCell& At(Coord pos) { return Cells.at(pos.Y).at(pos.X); }
        [[nodiscard]] constexpr const TCell& At(Coord pos) const { return Cells.at(pos.Y).at(pos.X); }
    };
}