#include "Pets/Combat/PreyProvider.h"
#include "Pets/Pets/Pets.h"

namespace Pets::PreyProvider {
	HuntCombatant GetPrey() {
        static size_t kindIndex = static_cast<size_t>(PetKind::COUNT);
        auto kind = static_cast<PetKind>(kindIndex--);
        if(kindIndex < 1) kindIndex = static_cast<size_t>(PetKind::COUNT);
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