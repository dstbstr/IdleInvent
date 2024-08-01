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

    struct Odds {
        f32 Artifact{0};
        f32 Success{0};
        f32 Nothing{0};
        f32 Tragedy{0};
    };

    Project CreateExpedition(Invention latestInvention);
    ExpeditionOutcome GetExpeditionOutcome(Expeditions expedition, bool artifactFound, Modifier modifier);
    Odds GetExpeditionOdds(Expeditions expedition, Modifier modifier);
    Odds ToRelativeOdds(const Odds& odds);

    std::string ToString(Expeditions expedition);
    std::optional<Expeditions> ExpeditionFromString(const std::string& str);

    constexpr std::string ToString(ExpeditionOutcome outcome) {
        switch(outcome) {
        case ExpeditionOutcome::Artifact: return "Artifact";
        case ExpeditionOutcome::Resources: return "Resources";
        case ExpeditionOutcome::Nothing: return "Nothing";
        case ExpeditionOutcome::Trajedy: return "Trajedy";
        }
        return "Unknown";
    }
} // namespace Invent