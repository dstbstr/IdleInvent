#include "CommonTest.h"

#include "InventLib/Projects/Population.h"

namespace Invent {
    TEST(PopulationCap, TimeCost_AfterLevel_IsGreater) {
        auto zero = GetPopulationCapacityProject(10);
        auto one = GetPopulationCapacityProject(100);
        EXPECT_TRUE(one.TimeCost > zero.TimeCost);
    }

    TEST(PopulationCap, ResourceCost_AfterLevel_IsGreater) {
        auto zero = GetPopulationCapacityProject(10);
        auto one = GetPopulationCapacityProject(100);

        for(const auto& [name, res]: zero.ResourceCost) {
            if(res.Current < 10) continue; // Skip resources that are too small to be meaningful
            ASSERT_TRUE(one.ResourceCost.at(name).Current > zero.ResourceCost.at(name).Current);
        }
    }
} // namespace Invent