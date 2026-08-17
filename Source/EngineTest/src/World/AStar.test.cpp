#include "CommonTest.h"
#include "World/AStar.h"
#include "World/World.h"

namespace World {
	struct AStarTest : public ::testing::Test {
    };

    std::array<CellCoord, 4> GetNeighbors(CellCoord node) {
        return SquareTopology::GetNeighbors(node);
    }

	TEST_F(AStarTest, Start_MatchesEnd_SingleElementPath) {
        CellCoord node{0, 0};
    
		auto path = AStar(node, node, GetNeighbors);
        ASSERT_NE(path, std::nullopt);
        auto p = path.value();
        ASSERT_EQ(p.size(), 1);
        ASSERT_EQ(p[0], node);
	}

    TEST_F(AStarTest, Start_WithNoNeighbors_ReturnsNullopt) {
        CellCoord start{0, 0};
        CellCoord end{1, 1};
        auto noNeighbors = [](CellCoord) { return std::array<CellCoord, 0>{}; };
        auto path = AStar(start, end, noNeighbors);

        ASSERT_EQ(path, std::nullopt);
    }

    TEST_F(AStarTest, Start_Adjacent_HasPathOf2) {
        CellCoord start{0, 0};
        CellCoord end{1, 0};
        auto path = AStar(start, end, GetNeighbors);

        ASSERT_NE(path, std::nullopt);
        auto p = path.value();

        ASSERT_EQ(p.size(), 2);
        ASSERT_EQ(p[0], start);
        ASSERT_EQ(p[1], end);
    }

    TEST_F(AStarTest, Start_AcrossManySteps_FindsPath) { 
        CellCoord start{0, 0};
        CellCoord end{3, 0};
        auto path = AStar(start, end, GetNeighbors);
        ASSERT_NE(path, std::nullopt);
        auto p = path.value();
        ASSERT_EQ(p.size(), 4);
        ASSERT_EQ(p[0], start);
        ASSERT_EQ(p[3], end);
    }
}