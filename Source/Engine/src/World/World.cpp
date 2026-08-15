#include "World/World.h"
#include <algorithm>

namespace World {
    static_assert(Coord{0, 0} == Coord{0, 0});
    static_assert(Coord{0, 1} != Coord{0, 0});

    static_assert(LocalPos{0.0f, 0.0f} == LocalPos{0.0f, 0.0f});
    static_assert(LocalPos{0.0f, 1.0f} != LocalPos{0.0f, 0.0f});

    constexpr bool SquareTopologyTests() {
        SquareTopology top;
        auto n = top.GetNeighbors({0, 0});
        if(n.size() != 4) return false;
        if(std::find(n.begin(), n.end(), Coord{-1, 0}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), Coord{1, 0}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), Coord{0, -1}) == n.end()) return false;
        if(std::find(n.begin(), n.end(), Coord{0, 1}) == n.end()) return false;

        auto a = Coord{1, 1};
        auto b = Coord{1, 2};
        if(!top.IsNeighbor(a, b)) return false;
        if(!top.IsNeighbor(b, a)) return false;

        if(top.MinDistance(a, a) != 0) return false;
        if(top.MinDistance(a, b) != 1) return false;

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

    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, 0.0f}) == WorldLocation{{0, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {1.0f, 1.0f}) == WorldLocation{{0, 0}, {1.0f, 1.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {16.0f, 0.0f}) == WorldLocation{{1, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, 16.0f}) == WorldLocation{{0, 1}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {-16.0f, 0.0f}) == WorldLocation{{-1, 0}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {0.0f, -16.0f}) == WorldLocation{{0, -1}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {32.0f, 32.0f}) == WorldLocation{{2, 2}, {0.0f, 0.0f}});
    static_assert(Offset<16, 16>({{0, 0}, {0.0f, 0.0f}}, {-32.0f, -32.0f}) == WorldLocation{{-2, -2}, {0.0f, 0.0f}});

} // namespace World
