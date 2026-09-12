#pragma once

#include "Pets/Pets/Pets.h"
#include "Pets/Pets/PreyDetails.h"

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>
#include <variant>

namespace Pets {
    struct PartyStats {
        u32 Attack{0};
        u32 Piercing{0};
    };

    struct PreyStats {
        PetKind Kind{};
        BattleDetails Battle{};
        s32 CurrentHp{0};
        BaseTime FleeTime{};
    };

    struct HuntCombatant {
        BaseTime ActionInterval{0};
        std::variant<PartyStats, PreyStats> Stats;
    };

    enum struct HuntFaction : u8 { Party, Prey };
}