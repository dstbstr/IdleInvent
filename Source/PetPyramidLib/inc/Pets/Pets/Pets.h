#pragma once

#include "Instrumentation/Logging.h"

#include <array>
#include <optional>
#include <string_view>

namespace Pets {
    enum struct PetKind {
        Unset,
        Hero,

        // Dogs
        Bloodhound,
        Labrador,
        Poodle,
        Chihuahua,

        // Cats
        HouseCat,
        Sabertooth,
        Lion,
        Tiger,
        Leapord,

        // Birds 
        Robin,
        Parrot,
        Stork,
        Pelacin,
        Ostrich,
        Penguin,
        
        // Rodents 
        Raccoon,
        Rat,
        Squirel,
        Hamster,
        Ferret,

        // Plants 
        Cactus,
        Petunias,
        VenusFlytrap,
        MonkeyPuzzleTree,
        Bonzai,
        SanboxTree,
        Seaweed,

        // Food Animals 
        Pig,
        Cow,
        Chicken,
        Sheep,
        Goat,
        Rabbit,
        
        // Fish 
        RedHerring,
        GoldFish,
        
        // Sea Creatures 
        Octopus,
        StarFish,
        SeaHorse,
        Anenome,
        Shrimp,
        Crab,
        Clam,
        Whale,
        Shark,
        
        //Bugs 
        LadyBug,
        Centipede,
        Spider,
        Scorpion,
        Mantis,
        DragonFly,
        
        // Mythological 
        Dragon,
        WorldTree,
        Fairy,
        Goblin,
        Ghost,
        Phoenix,
        
        //Inanament Objects 
        GarbageCan,
        Lightbulb,
        Plate,
        Book,
        Robot,
        Teapot,
        MoneyBag,
        
        // Vehicles 
        Car,
        Motorcycle,
        Truck,
        Bus,
        Plane,
        Train,
        Bike,
        Skateboard,
        Ship,
        Boat,
        
        // Reptile / Amphibian 
        Turtle,
        Snake,
        Lizard,
        Salamander,
        Toad,

        COUNT
    };

    struct OwnedPet {
        PetKind Kind{};
        u32 Level{1};
        u32 Experience{};
    };

    using PetSlots = std::array<std::optional<OwnedPet>, static_cast<size_t>(PetKind::COUNT)>;

    struct PetRoster {
        PetSlots Pets{};

        std::optional<OwnedPet>& operator[](PetKind kind) {
            auto index = static_cast<size_t>(kind);
            DR_ASSERT_MSG(index > 0 && index < static_cast<size_t>(PetKind::COUNT), "Invalid PetKind");
            return Pets[index];
        }
        const std::optional<OwnedPet>& operator[](PetKind kind) const {
            auto index = static_cast<size_t>(kind);
            DR_ASSERT_MSG(index > 0 && index < static_cast<size_t>(PetKind::COUNT), "Invalid PetKind");
            return Pets[index];
        }
    };

    std::string_view ToString(PetKind kind);
}