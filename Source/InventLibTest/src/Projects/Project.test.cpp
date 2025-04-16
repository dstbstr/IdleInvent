#include "CommonTest.h"

#include "InventLib/Projects/Project.h"

namespace Invent {
    struct ProjectTest : public ::testing::Test {
        Project project{};
        InventResourceCollection EnoughResources{};
        BaseTime EnoughTime = OneMinute;

        void SetUp() override { 
            InventResourceCollection cost{};
            for(auto id: GetSecondaryResourceIds()) {
                cost[id].Current = 10;
                EnoughResources[id].Current = 10;
            }
            project.ResourceCost = cost;
            project.TimeCost = EnoughTime;
        }
    };

    TEST_F(ProjectTest, IsComplete_WithEnoughResourcesAndTime_ReturnsTrue) {
        project.ResourceProgress = EnoughResources;
		project.TimeProgress = EnoughTime;
		ASSERT_TRUE(project.IsComplete());
    }

    TEST_F(ProjectTest, IsComplete_WhenTimeProgressIsLessThanTimeCost_ReturnsFalse) {
        project.ResourceProgress = EnoughResources;
		ASSERT_FALSE(project.IsComplete());
	}

    TEST_F(ProjectTest, IsComplete_MissingResources_ReturnsFalse) {
        project.ResourceProgress = InventResourceCollection{};
        project.TimeProgress = EnoughTime;
        ASSERT_FALSE(project.IsComplete());
    }

    TEST_F(ProjectTest, IsComplete_MissingOneResource_ReturnsFalse) {
        project.ResourceProgress = EnoughResources;
		project.ResourceProgress[2].Current = 0;
		project.TimeProgress = EnoughTime;
		ASSERT_FALSE(project.IsComplete());
    }

} // namespace Invent