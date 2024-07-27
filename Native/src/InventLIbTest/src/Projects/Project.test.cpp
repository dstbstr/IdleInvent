#include "CommonTest.h"

#include "InventLib/Projects/Project.h"

namespace Invent {
    struct ProjectTest : public ::testing::Test {
        Project project{};
        ResourceCollection EnoughResources{};
        BaseTime EnoughTime = OneMinute;

        void SetUp() override { 
            ResourceCollection cost{};
            for(auto resource: SecondaryResources()) {
                cost[resource].Current = 10;
                EnoughResources[resource].Current = 10;
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
        project.ResourceProgress = ResourceCollection{};
        project.TimeProgress = EnoughTime;
        ASSERT_FALSE(project.IsComplete());
    }

    TEST_F(ProjectTest, IsComplete_MissingOneResource_ReturnsFalse) {
        project.ResourceProgress = EnoughResources;
		project.ResourceProgress[ResourceName::Knowledge].Current = 0;
		project.TimeProgress = EnoughTime;
		ASSERT_FALSE(project.IsComplete());
    }

} // namespace Invent