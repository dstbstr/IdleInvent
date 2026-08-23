#include "World/World.h"
#include <algorithm>

namespace World {
    static_assert(ChunkCoord{0, 0} == ChunkCoord{0, 0});
    static_assert(ChunkCoord{0, 1} != ChunkCoord{0, 0});

    static_assert(LocalPos{0.0f, 0.0f} == LocalPos{0.0f, 0.0f});
    static_assert(LocalPos{0.0f, 1.0f} != LocalPos{0.0f, 0.0f});

    constexpr bool SquareTopologyTests() {
        auto n = SquareTopology::GetNeighbors({0, 0});
        if(n.size() != 4) return false;
        if(std::find(n.begin(), n.end(), CellCoord{-1, 0}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), CellCoord{1, 0}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), CellCoord{0, -1}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), CellCoord{0, 1}) == n.end()) return false;

        auto a = CellCoord{1, 1};
        auto b = CellCoord{1, 2};
        if(!SquareTopology::IsNeighbor(a, b)) return false;
        if(!SquareTopology::IsNeighbor(b, a)) return false;

        if(MDistance(a, a) != 0) return false;
        if(MDistance(a, b) != 1) return false;

        return true;
    }

    static_assert(SquareTopologyTests());

    static_assert(Chunk<int, 3, 2>::Width == 3);
    static_assert(Chunk<int, 3, 2>::Height == 2);
    static_assert(Chunk<int, 3, 3>::Contains({0, 0}));
    static_assert(Chunk<int, 3, 3>::Contains({2, 2}));

    static_assert(Chunk<int, 3, 3>::Contains({0, 3}) == false);
    static_assert(Chunk<int, 3, 3>::Contains({3, 0}) == false);
    static_assert(Chunk<int, 3, 3>::Contains({3, 3}) == false);
    static_assert(Chunk<int, 3, 3>::Contains({-1, 0}) == false);
    static_assert(Chunk<int, 3, 3>::Contains({0, -1}) == false);
    
    // Uses integer division to find center
    static_assert(Chunk<int, 3, 3>::CenterCell() == CellCoord{1, 1});
    static_assert(Chunk<int, 4, 4>::CenterCell() == CellCoord{2, 2});
    static_assert(Chunk<int, 3, 4>::Size() == CellSize{3, 4});

    constexpr bool ChunkTests() { 
        Chunk<int, 3, 2> chunk{};
        auto& cell = chunk.At({1, 1});
        if(cell != 0) return false;
        cell = 42;
        if(chunk.At({1, 1}) != 42) return false;

        const auto& constChunk = chunk;
        const auto& constCell = constChunk.At({0, 0});

        if(constCell != 0) return false;

        return true;
    }

    static_assert(ChunkTests());
} // namespace World
