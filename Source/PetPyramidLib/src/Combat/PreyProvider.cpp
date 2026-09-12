#include "Pets/Combat/PreyProvider.h"
#include "Pets/Pets/Pets.h"
#include "Pets/Pets/PreyDetails.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/IRandom.h>

#include <array>

namespace Pets::PreyProvider {
	HuntCombatant GetPrey() {
        constexpr std::array Valid{
            PetKind::Bloodhound, PetKind::Labrador, PetKind::Poodle, PetKind::Chihuahua, PetKind::HouseCat
        };

        struct Candidate {
            const PreyDetails* Details{};
            f64 CumulativeWeight{};
        };

        std::array<Candidate, Valid.size()> candidates{};
        auto totalWeight = 0.0;

        for(size_t i = 0; i < Valid.size(); i++) {
            auto& details = Details::GetPrey(Valid[i]);
            totalWeight += 1.0 / static_cast<f64>(details.Encounter.Rarity);
            candidates[i] = {&details, totalWeight};
        }

        auto& rand = ServiceLocator::Get().GetRequired<IRandom>();
        auto roll = rand.GetNextFloat() * static_cast<f32>(totalWeight);
        auto* selected = candidates.back().Details;

        for(auto candidate : candidates) {
            if(roll < candidate.CumulativeWeight) {
                selected = candidate.Details;
                break;
            }
        }

        const auto& details = *selected;
        return HuntCombatant{
            .ActionInterval = OneSecond,
            .Stats = PreyStats{
                .Kind = details.Kind,
                .Battle = details.Battle,
                .CurrentHp = details.Battle.MaxHp,
                .FleeTime = details.Battle.FleeTime
            }
        };
	}
}