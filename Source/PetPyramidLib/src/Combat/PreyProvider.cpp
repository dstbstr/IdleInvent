#include "Pets/Combat/PreyProvider.h"
#include "Pets/Pets/Pets.h"
#include "Pets/Pets/PreyDetails.h"

#include <array>

namespace Pets::PreyProvider {
	HuntCombatant GetPrey() {
        constexpr std::array Valid{
            PetKind::Bloodhound, PetKind::Labrador, PetKind::Poodle, PetKind::Chihuahua, PetKind::HouseCat
        };

        static size_t kindIndex = 0;
        auto kind = Valid[kindIndex];
        kindIndex = (kindIndex + 1) % Valid.size();
        auto details = Details::GetPrey(kind);
        return HuntCombatant{
            .ActionInterval = OneSecond,
            .Stats = PreyStats{
                .Kind = kind,
                .Battle = details.Battle,
                .CurrentHp = details.Battle.MaxHp,
                .FleeTime = details.Battle.FleeTime
            }
        };
	}
}