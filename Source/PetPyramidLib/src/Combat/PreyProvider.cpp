#include "Pets/Combat/PreyProvider.h"
#include "Pets/Pets/Pets.h"

namespace Pets::PreyProvider {
	HuntCombatant GetPrey() {
        static size_t kindIndex = 0;
        auto kind = static_cast<PetKind>(kindIndex + 2); // skip Unset and Hero
        kindIndex = (kindIndex + 1) % 3; // just first 3 kinds for now
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