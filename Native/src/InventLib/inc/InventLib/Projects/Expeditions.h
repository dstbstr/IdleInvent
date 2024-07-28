#pragma once

#include "InventLib/Projects/Project.h"	
#include "InventLib/Projects/Research.h"

#include <vector>

namespace Invent {
    enum struct Expeditions {
        //// Followers
        ConvertAVillage,
        EstablishBoundaries,
        WageWar,
        ExpandNation,
        EstablishTreaty,
        CombineNations,
        CommunicateWithAliens,

        //// Knowledge
        RecordHistory,
        CreateSchoolOfThought,
        CreateWrittenLanguage,
        AuthorBook,
        OpenUniversity,
        ImproveHttp,
        QuantumBreakthrough,

        //// Money
        FindNewResources,
        TradeWithNeighbours,
        MintCoins,
        EstablishBank,
        EstablishCommodity,
        CombineCurrencies,
        TradeWithAliens,

        //// Power
        CreateTools,
        EstablishFarm,
        InventMachine,
        BuildStructure,
        FindPowerSource,
        ImproveNuclearPower,
        LaunchSpacecraft,

        FIRST = ConvertAVillage,
        LAST = LaunchSpacecraft
    };

    enum struct ExpeditionOutcome {
        Artifact,
        Resources,
        Nothing,
        Trajedy
    };

    Project CreateExpedition(Invention latestInvention);
    ExpeditionOutcome GetExpeditionOutcome(Expeditions expedition, bool artifactFound); // equipment/items

    std::string ToString(Expeditions expedition);
    std::optional<Expeditions> ExpeditionFromString(const std::string& str);
} // namespace Invent