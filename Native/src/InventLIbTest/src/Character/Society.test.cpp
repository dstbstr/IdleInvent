#include "CommonTest.h"

#include "InventLib/Mechanics/Effect.h"
#include "InventLib/Character/Society.h"
#include "InventLib/Settings/GameSettings.h"
#include "InventLib/Projects/Population.h"

namespace Invent {
    TEST(SocietyTest, StartLife_WithPower_SetsPrimaryProgress) {
        Society society{};
        GameSettings settings{};

        society.Start(ResourceName::Power, settings);

        ASSERT_EQ(2.0, society.CurrentLife.ResourceProgressions[ResourceName::Primary].GetProgress(OneSecond));
    }

    TEST(SocietyTest, PopulationProjects_AfterFinishingExpansion_ContainsOneIncreaseProject) {
        Society society{};
        GameSettings settings{};
        society.Start(ResourceName::Power, settings);

        auto& populationProjects = society.CurrentLife.Projects[ProjectType::Population];
        auto expanisonProject = std::find_if(populationProjects.begin(), populationProjects.end(), [](const auto& project) {
            return project.Name == Population::CapacityName;
        });
        ASSERT_NE(populationProjects.end(), expanisonProject);
        auto& p = *expanisonProject;
        p.ResourceProgress = p.ResourceCost;
        p.TimeProgress = p.TimeCost;

        society.Tick(OneSecond);

        ASSERT_EQ(2, populationProjects.size());
    }

} // namespace Invent