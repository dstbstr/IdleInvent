#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/Resources/Resource.h"

#include <string>

namespace Invent {
    enum struct ProjectType {Research, Build, Explore, Population};
    std::vector<ProjectType> AllProjectTypes();

    std::string ToString(ProjectType type);

    struct Project {
        std::string Name{};
        std::string Description{};
        std::string EffectDescription{};
        ProjectType Type{ProjectType::Research};
        size_t CurrentWorkers{0};

        BaseTime TimeProgress{0};
        BaseTime TimeCost{0};

        ResourceCollection ResourceProgress{};
        ResourceCollection ResourceCost{};

        bool IsComplete() const;

        void Tick(BaseTime elapsed);
        void Invest(ResourceCollection& resources);

        constexpr bool operator==(const Project& other) const {
			return Name == other.Name;
		}
    };
}