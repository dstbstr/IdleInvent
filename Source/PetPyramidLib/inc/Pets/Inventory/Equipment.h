#pragma once

namespace Pets {
    enum struct EquipmentSlot {
        Head,
        Torso,
        Legs,
        Feet,
        Hands,
        Face,
        Weapon,
        Shield
    };

    enum struct Equipment {
        // Head
        DunceCap,
        PropellerHat,
        WizardHat,
        FishHead,
        Sombrero,
        NachoHat,
        EarMuffs,
        ClownWig,

        // Torso
        BandTshirt,
        SweaterVest,
        Bullseye,
        ChestBurster, // double strength, will die in 30 minutes
        Cape,

        // Legs
        Shorts,
        ShinGuards,
        ChickenLegs,
        PegLeg,

        // Hands
        Mittens,
        PowerGlove,
        RingPop,
        MoodRing,
        FriendshipBracelet,

        // Feet
        Sandles,
        ClownShoes,
        RubberBoot,
        Shackles,
        BallAndChain,
        Crocs,
        AnkleMonitor,
        RollerSkates,
        HighHeels,

        // Face
        Sunglasses,
        Monocle,
        Snorkle,
        FakeMustache,
        HalloweenMask,
        Braces,
        NoseRing,
        ClownNose,

        // Weapon
        WiffleBat,
        CoinSock,
        BrassKnuckles,
        RubberChicken,
        AirHorn,
        Pinwheel,
        SquirtGun,

        // Shield
        TrashCanLid,
        Candleabra,
        PizzaBox,
        Umbrella,
        Stuffy,
        FogMachine
    };
}