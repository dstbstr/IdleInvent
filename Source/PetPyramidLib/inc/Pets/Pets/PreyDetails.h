#pragma once

#include "Pets/Pets/Pets.h"
#include <GameState/GameTime.h>

namespace Pets {
    struct BattleDetails {
        s32 MaxHp{};
        s32 Armor{};
        f32 Dodge{};
        f32 CaptureHpLevel{};
        f32 CaptureChance{};
        u32 Gold{};
        u32 Xp{};
        BaseTime FleeTime{};    
    };

    struct EncounterDetails {
        u32 Rarity{};

    };

    struct PreyDetails {
        PetKind Kind{};
        BattleDetails Battle{};
        EncounterDetails Encounter{};
    };

    namespace Details {
        [[nodiscard]] const PreyDetails& GetPrey(PetKind kind);
        [[nodiscard]] bool ContainsPrey(PetKind kind);
    } // namespace Details
}