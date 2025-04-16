#pragma once

#include "InventLib/Projects/Project.h"

#include <Resources/Resource.h>

#include <unordered_map>
#include <vector>
#include <optional>

namespace Invent {
    enum struct Invention {
        // Followers
        Totem,
        Warlord,
        Military,
        Nation,
        Diplomacy,
        WorldGovernment,
        AlienAlliance,
        // Knowledge
        Tradition,
        Philosophy,
        Writing,
        Printing,
        Education,
        Internet,
        QuantumEngineering,
        // Money
        HunterGather,
        Barter,
        Coins,
        Banking,
        StockMarket,
        GlobalEconomy,
        AlienVisitors,
        // Power
        StoneTools,
        Agriculture,
        SimpleMachines,
        Construction,
        Electricity,
        NuclearPower,
        SpaceTravel
    };

    std::string ToString(Invention invention);
    std::optional<Invention> InventionFromString(const std::string& invention);
    std::optional<Project> GetResearchProject(ResourceName resourceName, size_t level);
} // namespace Invent