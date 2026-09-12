#pragma once

#include <array>

namespace Pets {
    enum struct CombatItemKind {
        Distraction,
        Net,
        AtkPotion,
        SpdPotion,
        PiercePotion,
        Poison
    };

    enum struct FieldItemKind {
        Bait,
        PetRepelent,
        Hint
    };

    constexpr auto UsableFieldItems = std::array{FieldItemKind::Bait, FieldItemKind::PetRepelent};

    /*
    enum struct Items {
        EnergyDrink,
        Cookie,
        Pie,
        Cake,
        Lotion,
        Grease,
        PeanutOil,
        GhostPepper,
        PhoneCharger,
        Haggis,
        HairGel,
        WaterBalloon,
        CandyCorn,
        ChewedBubbleGum,
        SleepingPill,
        FlamingPooBag,
        SnoCone,
        FriedButter
    };
    */
}