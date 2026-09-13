#include "Pets/Pets/PreyDetails.h"

#include "Pets/Character/PartyResolver.h"

#include <GameState/GameTime.h>
#include <optional>

namespace {
    using namespace Pets;

    constexpr auto DetailMap = []{
        std::array<std::optional<PreyDetails>, static_cast<size_t>(PetKind::COUNT)> details{};

        // Dogs
        details[static_cast<size_t>(PetKind::Bloodhound)] = PreyDetails{
            .Kind = PetKind::Bloodhound,
            .Battle {
                .MaxHp = 100,
                .Armor = 3,
                .Dodge = 0.2f,
                .CaptureHpLevel = 0.25f,
                .CaptureChance = 0.5f,
                .Gold = 100,
                .Xp = 50,
                .FleeTime = OneSecond * 30
            },
            .Encounter {
                .Rarity = 1
            }
        };
        
        details[static_cast<size_t>(PetKind::Labrador)] = PreyDetails{
            .Kind = PetKind::Labrador,
            .Battle {
                .MaxHp = 80,
                .Armor = 2,
                .Dodge = 0.1f,
                .CaptureHpLevel = 0.2f,
                .CaptureChance = 0.4f,
                .Gold = 80,
                .Xp = 40,
                .FleeTime = OneSecond * 25
            },
            .Encounter {
                .Rarity = 2,
                .IsEligible = [](const EncounterContext& context) { 
                    return context.Party.Pets.size() >= 3;
                },
                .PrereqDesc = "Requires 2 or more pets in party"
            }
        };

        details[static_cast<size_t>(PetKind::Poodle)] = PreyDetails{
            .Kind = PetKind::Poodle,
            .Battle {
                .MaxHp = 60,
                .Armor = 1,
                .Dodge = 0.15f,
                .CaptureHpLevel = 0.15f,
                .CaptureChance = 0.3f,
                .Gold = 60,
                .Xp = 30,
                .FleeTime = OneSecond * 20
            },
            .Encounter {
                .Rarity = 3
            }
        };

        details[static_cast<size_t>(PetKind::Chihuahua)] = PreyDetails{
            .Kind = PetKind::Chihuahua,
            .Battle {
                .MaxHp = 40,
                .Armor = 0,
                .Dodge = 0.25f,
                .CaptureHpLevel = 0.1f,
                .CaptureChance = 0.2f,
                .Gold = 40,
                .Xp = 20,
                .FleeTime = OneSecond * 15
            },
            .Encounter{
                .Rarity = 4
            }
        };

        // Cats
        details[static_cast<size_t>(PetKind::HouseCat)] = PreyDetails{
            .Kind = PetKind::HouseCat,
            .Battle {
                .MaxHp = 50,
                .Armor = 1,
                .Dodge = 0.2f,
                .CaptureHpLevel = 0.15f,
                .CaptureChance = 0.1f,
                .Gold = 50,
                .Xp = 25,
                .FleeTime = OneSecond * 20
            },
            .Encounter {
                .Rarity = 5,
                .IsEligible = [](const EncounterContext& context) {
                    return context.Party.Pets.size() == 1;
                },
                .PrereqDesc = "Must fight alone"
            }
        };

            /*
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
*/

        return details;
    }();

} // namespace

namespace Pets::Details {

    [[nodiscard]] const PreyDetails& GetPrey(PetKind kind) { return *DetailMap.at(static_cast<size_t>(kind)); }

    [[nodiscard]] bool ContainsPrey(PetKind kind) { return DetailMap.at(static_cast<size_t>(kind)).has_value(); }
} // namespace Pets::Details