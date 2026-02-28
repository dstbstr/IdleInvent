#include "CommonTest.h"
#include "InventLib/Character/Life.h"

#include "InventLib/Character/Society.h"
#include "InventLib/Effects/Effect.h"
#include "InventLib/GameState/GameSettings.h"
#include "InventLib/Projects/Population.h"
#include "InventLib/Projects/Project.h"

namespace Invent {
    struct LifeTest : public ::testing::Test {
        Society society{};
        Life* life{nullptr};
        GameSettings settings{};

        void SetUp() override {
            society.Start(ResourceName::Money, settings);
            life = &society.CurrentLife;
        }
    };

    TEST_F(LifeTest, Projects_OnConstruction_ContainsFirstInvention) {
        ASSERT_EQ(1ull, life->Projects.at(ProjectType::Research).size());
    }

    TEST_F(LifeTest, Projects_OnConstruction_ContainsPopulationProjects) {
        ASSERT_EQ(2ull, life->Projects.at(ProjectType::Population).size());
    }

    TEST_F(LifeTest, PrimaryResource_AfterOneSecond_IsMoreThanZero) {
        life->Tick(OneSecond);
        ASSERT_TRUE(life->Resources[ResourceName::Primary].Current > 0);
    }

    TEST_F(LifeTest, CurrentPopulation_AfterFinishingPopulationIncreaseProject_InreasesByOne) {
        auto& projects = life->Projects.at(ProjectType::Population);
        auto expected = Project{.Name = Invent::Population::PopulationName, .Type = ProjectType::Population};
        auto project = std::find(projects.begin(), projects.end(), expected);
        ASSERT_NE(projects.end(), project);

        project->ResourceProgress = project->ResourceCost;
        project->TimeProgress = project->TimeCost;

        auto original = life->CurrentPopulation;
        life->Tick(OneSecond);

        ASSERT_EQ(original + 1, life->CurrentPopulation);
    }

    TEST_F(LifeTest, PopulationCap_AfterFinishingPopulationCapProject_IsMultipliedBy10) {
        auto& projects = life->Projects.at(ProjectType::Population);
        auto expected = Project{.Name = Invent::Population::CapacityName, .Type = ProjectType::Population};
        auto project = std::find(projects.begin(), projects.end(), expected);
        ASSERT_NE(projects.end(), project);

        project->ResourceProgress = project->ResourceCost;
        project->TimeProgress = project->TimeCost;

        auto original = life->MaxPopulation;
        life->Tick(OneSecond);

        ASSERT_EQ(original * 10, life->MaxPopulation);
    }

    TEST_F(LifeTest, Project_AfterCompletion_RemovesProject) {
        auto& projects = life->Projects.at(ProjectType::Research);
        projects.clear();
        auto project = Project{.Name = ToString(Invention::Coins), .Type = ProjectType::Research};
        projects.push_back(project);

        life->Tick(OneSecond);

        ASSERT_EQ(projects.end(), std::find(projects.begin(), projects.end(), project));
    }
} // namespace Invent