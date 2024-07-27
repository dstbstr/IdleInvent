#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Mechanics/Progression.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/ResourceConversion.h"
#include "InventLib/Projects/Project.h"
#include "InventLib/Projects/Research.h"
#include "InventLib/Projects/Building.h"

#include "Core/NumTypes.h"

#include <vector>
#include <map>

namespace Invent {
    struct Effect;
    struct Society;

    struct Life {
        Life(Society* society);

        std::unordered_map<ResourceName, Progression> ResourceProgressions{};
        std::unordered_map<ProjectType, Modifier> ProjectTimeCostModifiers{};
        std::unordered_map<ProjectType, Modifier> ProjectResourceCostModifiers{};
        std::vector<ResourceConversion> ResourceConverters{};

        std::vector<Invention> Inventions{};
        std::map<Building, size_t> Buildings{};
        std::unordered_map<ProjectType, std::vector<Project>> Projects{};
        size_t MaxProjects{3};
        size_t AvailableWorkers{MaxProjects};

        ResourceCollection Resources{};
        size_t CurrentPopulation{1};
        size_t MaxPopulation{10};
        size_t CurrentTimeShards{0};
        size_t MaxTimeShards{1 * 60 * 60 * 4}; // 4 hours
        Modifier TickModifier{.Add = 0, .Mul = 1.0F};

        void Tick(BaseTime elapsed);
        void Work(BaseTime elapsed = OneSecond);

        void ApplyEffect(const Effect& effect);
        void SortProjects();

    private:
        Society* m_Society{nullptr};
    };
} // namespace Invent