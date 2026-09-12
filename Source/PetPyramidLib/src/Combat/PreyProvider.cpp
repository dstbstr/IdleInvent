#include "Pets/Combat/PreyProvider.h"
#include "Pets/Combat/HuntManager.h"
#include "Pets/Pets/Pets.h"
#include "Pets/Pets/PreyDetails.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/IRandom.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

namespace {
    std::vector<Pets::PetKind> GetValidPrey(const Pets::EncounterContext& context) {
        std::vector<Pets::PetKind> valid{};
        for(auto kind : Pets::GetNonHeroPets()) {
            if(Pets::Details::ContainsPrey(kind)) {
                auto details = Pets::Details::GetPrey(kind);
                if(!details.Encounter.IsEligible || details.Encounter.IsEligible(context)) {
                    valid.push_back(kind);
                }
            }
        }

        return valid;
    }
}

namespace Pets::PreyProvider {
	HuntCombatant GetPrey(const EncounterContext& context) {
        struct Candidate {
            const PreyDetails* Details{};
            f64 CumulativeWeight{};
        };

        auto valid = GetValidPrey(context);
        auto candidates = std::vector<Candidate>{};
        candidates.reserve(valid.size());
        auto totalWeight = 0.0;
        auto baitActive =
            std::ranges::contains(context.FieldEffects, Pets::FieldItemKind::Bait, &Pets::FieldEffect::Kind);
        auto repellentActive =
            std::ranges::contains(context.FieldEffects, Pets::FieldItemKind::PetRepelent, &Pets::FieldEffect::Kind);

        for(size_t i = 0; i < valid.size(); i++) {
            auto& details = Details::GetPrey(valid[i]);
            DR_ASSERT_MSG(details.Encounter.Rarity > 0, "Prey rarity must be greater than 0");

            auto weight = 1.0 / static_cast<f64>(details.Encounter.Rarity);
            if(baitActive) weight = std::sqrt(weight);
            if(repellentActive && context.Roster[details.Kind].has_value()) weight *= 0.1f;
            
            totalWeight += weight;
            candidates.emplace_back(&details, totalWeight);
        }

        DR_ASSERT_MSG(!candidates.empty(), "No valid prey found");

        auto& rand = ServiceLocator::Get().GetRequired<IRandom>();
        auto roll = static_cast<f64>(rand.GetNextFloat()) * totalWeight;
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