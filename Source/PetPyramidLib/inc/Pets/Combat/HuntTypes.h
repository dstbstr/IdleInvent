#pragma once

#include <Combat/CombatTypes.h>
#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>

#include <variant>

namespace Pets {
    struct PartyStats {
        u32 Attack{0};
        u32 Piercing{0};
    };

    struct PreyStats {
        s32 CurrentHp{0};
        s32 MaxHp{0};
        s32 Armor{0};
        f32 Dodge{0.f};

        f32 CaptureHpLevel{0.f};
        f32 CaptureChance{0.f};

        u32 Gold{0};
        u32 Xp{0};
        BaseTime FleeTime{};
    };

    struct HuntCombatant {
        BaseTime ActionInterval{0};
        std::variant<PartyStats, PreyStats> Stats;
    };

    enum struct HuntFaction : u8 { Party, Prey };
}