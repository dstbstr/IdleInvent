#include "CommonTest.h"

#include "InventLib/Projects/Building.h"

namespace Invent {
    TEST(Building, TimeCost_AfterLevel_IsGreater) { 
        auto zero = GetBuildingProject(Building::Hammer, 0);
        auto one = GetBuildingProject(Building::Hammer, 1);
		EXPECT_TRUE(one.TimeCost > zero.TimeCost);
    }

    TEST(Building, ResourceCost_AfterLevel_IsGreater) {
        auto zero = GetBuildingProject(Building::Hammer, 0);
        auto one = GetBuildingProject(Building::Hammer, 1);

        for(const auto& [name, res]: zero.ResourceCost) {
            if(res.Current < 10) continue; // Skip resources that are too small to be meaningful
            ASSERT_TRUE(one.ResourceCost.at(name).Current > zero.ResourceCost.at(name).Current);
		}
    }
}