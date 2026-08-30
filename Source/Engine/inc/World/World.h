#pragma once
#include "Constexpr/ConstexprMath.h"
#include "Geometry/Geometry.h"

#include <Platform/NumTypes.h>

#include <array>
#include <cmath>

namespace World {
    struct WorldSpace;
    struct ChunkSpace;

    using ChunkCoord = Geometry::Point2<s32, WorldSpace>;
    using CellCoord = Geometry::Point2<s32, ChunkSpace>;
    using LocalPos = Geometry::Point2<f32, ChunkSpace>;
    using CellSize = Geometry::Size2<s32, ChunkSpace>;
    using Displacement = LocalPos;

    [[nodiscard]] constexpr CellCoord ToCellCoord(const LocalPos& local) {
        return {
            static_cast<s32>(std::floor(local.X)), 
            static_cast<s32>(std::floor(local.Y))
        };
    }

    [[nodiscard]] constexpr LocalPos GetCellCenter(const CellCoord& cell) {
        return {static_cast<f32>(cell.X) + 0.5f, static_cast<f32>(cell.Y) + 0.5f};
    }

    struct WorldLocation {
        ChunkCoord Chunk{};
        LocalPos Local{};
        constexpr auto operator<=>(const WorldLocation&) const = default;

        constexpr CellCoord ToCellCoord() const {
            return World::ToCellCoord(Local);
        }
    };

    namespace SquareTopology {
        constexpr std::array<CellCoord, 4> GetNeighbors(CellCoord pos) {
            return {
                CellCoord{pos.X - 1, pos.Y},
                CellCoord{pos.X + 1, pos.Y},
                CellCoord{pos.X, pos.Y - 1},
                CellCoord{pos.X, pos.Y + 1}
            };
        }

        [[nodiscard]] constexpr bool IsNeighbor(CellCoord a, CellCoord b) { return MDistance(a, b) == 1; }
    }

    template<typename TCell, size_t TWidth, size_t THeight>
    struct Chunk {
        static_assert(TWidth > 0 && THeight > 0, "Chunks must have positive dimensions");
        static constexpr size_t Width{TWidth};
        static constexpr size_t Height{THeight};

        [[nodiscard]] static constexpr CellCoord CenterCell() {
            return {static_cast<s32>(Width / 2), static_cast<s32>(Height / 2)};
        }
        [[nodiscard]] static constexpr CellSize Size() { return {static_cast<s32>(Width), static_cast<s32>(Height)}; }

        std::array<std::array<TCell, TWidth>, THeight> Cells{};

        [[nodiscard]] static constexpr bool Contains(CellCoord pos) {
            return pos.X >= 0 &&
                   pos.X < static_cast<s32>(Width) &&
                   pos.Y >= 0 &&
                   pos.Y < static_cast<s32>(Height);
        }

        [[nodiscard]] constexpr TCell& At(CellCoord pos) { return Cells.at(pos.Y).at(pos.X); }
        [[nodiscard]] constexpr const TCell& At(CellCoord pos) const { return Cells.at(pos.Y).at(pos.X); }

        constexpr void VisitCells(const auto& visitor) {
            for(size_t row = 0; row < Height; ++row) {
                for(size_t col = 0; col < Width; ++col) {
                    auto coord = CellCoord{static_cast<s32>(col), static_cast<s32>(row)};
                    visitor(coord, Cells.at(row).at(col));
                }
            }
        }

        constexpr void VisitCells(const auto& visitor) const {
            for(size_t row = 0; row < Height; ++row) {
                for(size_t col = 0; col < Width; ++col) {
                    auto coord = CellCoord{static_cast<s32>(col), static_cast<s32>(row)};
                    visitor(coord, Cells.at(row).at(col));
                }
            }        
        }
    };
}