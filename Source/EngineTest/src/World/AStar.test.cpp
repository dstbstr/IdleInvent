#include "CommonTest.h"
#include "World/AStar.h"
#include "World/World.h"

namespace World {
	struct AStarTest : public ::testing::Test {
    };

    std::array<Coord, 4> GetNeighbors(Coord node) {
        SquareTopology topology;
        return topology.GetNeighbors(node);
    }

	TEST_F(AStarTest, Start_MatchesEnd_SingleElementPath) {
        Coord node{0, 0};
    
		auto path = AStar(node, node, GetNeighbors);
        ASSERT_NE(path, std::nullopt);
        auto p = path.value();
        ASSERT_EQ(p.size(), 1);
        ASSERT_EQ(p[0], node);
	}

    TEST_F(AStarTest, Start_WithNoNeighbors_ReturnsNullopt) {
        Coord start{0, 0};
        Coord end{1, 1};
        auto noNeighbors = [](Coord) { return std::array<Coord, 0>{}; };
        auto path = AStar(start, end, noNeighbors);

        ASSERT_EQ(path, std::nullopt);
    }

    TEST_F(AStarTest, Start_Adjacent_HasPathOf2) {
        Coord start{0, 0};
        Coord end{1, 0};
        auto path = AStar(start, end, GetNeighbors);

        ASSERT_NE(path, std::nullopt);
        auto p = path.value();

        ASSERT_EQ(p.size(), 2);
        ASSERT_EQ(p[0], start);
        ASSERT_EQ(p[1], end);
    }

    TEST_F(AStarTest, Start_AcrossManySteps_FindsPath) { 
        Coord start{0, 0};
        Coord end{3, 0};
        auto path = AStar(start, end, GetNeighbors);
        ASSERT_NE(path, std::nullopt);
        auto p = path.value();
        ASSERT_EQ(p.size(), 4);
        ASSERT_EQ(p[0], start);
        ASSERT_EQ(p[3], end);
    }
}