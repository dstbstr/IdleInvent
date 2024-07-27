#pragma once

#include "InventLib/Projects/Project.h"

namespace Invent {
    namespace Population {
        constexpr auto PopulationName = "Populate";
        constexpr auto CapacityName = "Expand";
    }

    Project GetPopulationIncreaseProject(size_t currentPopulation);
    Project GetPopulationCapacityProject(size_t currentCapacity);
}