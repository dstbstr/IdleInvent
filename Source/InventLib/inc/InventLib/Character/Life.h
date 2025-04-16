#pragma once

#include "InventLib/GameState/GameSettings.h"
#include "InventLib/Projects/Project.h"
#include "InventLib/Projects/Research.h"
#include "InventLib/Resources/InventResources.h"

#include <GameState/GameTime.h>
#include <Mechanics/Progression.h>
#include <Resources/Resource.h>
#include <Resources/ResourceConversion.h>
#include <Platform/NumTypes.h>

#include <vector>
#include <map>

struct Effect;
namespace Invent {
    struct Society; // note, including Society.h here would create a circular dependency

    struct Life {
        Life(Society* society, const GameSettings& settings);

        std::unordered_map<ResourceName, Progression> ResourceProgressions{};
        std::unordered_map<ProjectType, Modifier> ProjectTimeCostModifiers{};
        std::unordered_map<ProjectType, Modifier> ProjectResourceCostModifiers{};
        Modifier BaseExplorationSuccessModifier{.Add = 0, .Mul = 1.0F};
        Modifier CurrentExplorationSuccessModifier {.Add = 0, .Mul = 1.0F};
        std::vector<ResourceConversion> ResourceConverters{};
        InventResourceCollection Resources{};

        std::vector<Invention> Inventions{};
        std::vector<std::string> Artifacts{};
        std::map<std::string, size_t> Buildings{};
        std::unordered_map<ProjectType, std::vector<Project>> Projects{};

        u8 WorkerDensity{5}; // Max Workers = CurrentPopulation / WorkerDensity
        size_t MaxWorkers{3};
        size_t AvailableWorkers{MaxWorkers};

        size_t CurrentPopulation{15};
        size_t MaxPopulation{30};
        size_t CurrentTimeShards{0};
        size_t MaxTimeShards{1 * 60 * 60 * 4}; // 4 hours
        
        Modifier TickModifier{.Add = 0, .Mul = 1.0F};
        void Tick(BaseTime elapsed);
        void Work(BaseTime elapsed = OneSecond);

        void ShiftWorkers();
        void ClearWorkers();
        void ApplyEffect(const Effect& effect);
        void SortProjects();

    private:
        Society* m_Society{nullptr};
        const GameSettings* m_Settings{nullptr};
    };
} // namespace Invent