#include "Pets/Combat/PreyProvider.h"
#include "Pets/Pets/Pets.h"

#include <array>

namespace Pets::PreyProvider {
	HuntCombatant GetPrey() {
        constexpr std::array Valid{
            PetKind::Bloodhound, PetKind::Labrador, PetKind::Poodle, PetKind::Chihuahua, PetKind::HouseCat
        };

        static size_t kindIndex = 0;
        auto kind = Valid[kindIndex];
        kindIndex = (kindIndex + 1) % Valid.size();
        return HuntCombatant{
            .ActionInterval = OneSecond,
            .Stats = PreyStats{
                .Kind = kind,
                .CurrentHp = 25,
                .MaxHp = 25,
                .Armor = 5,
                .Dodge = 0.1f,
                .CaptureHpLevel = 0.2f,
                .CaptureChance = 0.5f,
                .Gold = 100,
                .Xp = 50,
                .FleeTime = OneSecond * 20,
                .MaxFleeTime = OneSecond * 20
            }
        };
	}
}