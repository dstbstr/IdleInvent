#include "InventLib/Character/Society.h"

#include "InventLib/Mechanics/Effect.h"

#include <array>

namespace {
    using namespace Invent;

    struct SpecialtyEffects {
        std::array<Effect, 3> Benefits{};
        std::array<Effect, 2> Penalties{};
        ResourceName Specialty{ResourceName::Unset};
        ResourceName Weakness {ResourceName::Unset};

        Life ToLife(Society& society, const GameSettings& settings) const { 
            society.Specialty = Specialty;
            society.Weakness = Weakness;
            Life result {&society, settings};
            for(const auto& benefit: Benefits) {
				result.ApplyEffect(benefit);
			}
            for(const auto& penalty: Penalties) {
                result.ApplyEffect(penalty);
            }

            return result;
        }
    };

    constexpr auto iml = Effects::Increase::Mul::Large;
    constexpr auto imm = Effects::Increase::Mul::Medium;
    constexpr auto ims= Effects::Increase::Mul::Small;
    //constexpr auto ial = Effects::Increase::Add::Large;
    constexpr auto iam = Effects::Increase::Add::Medium;
    constexpr auto ias = Effects::Increase::Add::Small;

    //constexpr auto dml = Effects::Decrease::Mul::Large;
    constexpr auto dmm = Effects::Decrease::Mul::Medium;
    constexpr auto dms = Effects::Decrease::Mul::Small;
    //constexpr auto dal = Effects::Decrease::Add::Large;
    //constexpr auto dam = Effects::Decrease::Add::Medium;
    constexpr auto das = Effects::Decrease::Add::Small;

    constexpr SpecialtyEffects FollowerEffects{
        .Benefits =
            {{{EffectTarget::AllSecondaryRate, iml},
              {EffectTarget::BuildRate, imm},
              {EffectTarget::PopulationRate, ias}}},
        .Penalties =
            {{{EffectTarget::ResearchRate, dmm},
              {EffectTarget::ExploreRate, dmm}}},
        .Specialty = ResourceName::Followers,
        .Weakness = ResourceName::Money
    };
    constexpr SpecialtyEffects KnowledgeEffects{
        .Benefits = 
            {{{EffectTarget::ResearchRate, iml},
              {EffectTarget::ExploreRate, imm},
              {EffectTarget::AllSecondaryCap, ims}}},
        .Penalties = 
            {{{EffectTarget::AllSecondaryRate, dmm},
              {EffectTarget::BuildRate, dms}}},
        .Specialty = ResourceName::Knowledge,
        .Weakness = ResourceName::Power
    };
    constexpr SpecialtyEffects MoneyEffects{
        .Benefits =
            {{{EffectTarget::AllSecondaryCap, iml},
              {EffectTarget::PopulationRate, iam},
              {EffectTarget::BuildRate, ims}}},
        .Penalties = 
            {{{EffectTarget::ExploreRate, dmm},
              {EffectTarget::PrimaryRate, dms}}},
        .Specialty = ResourceName::Money,
        .Weakness = ResourceName::Followers
    };
    constexpr SpecialtyEffects PowerEffects{
        .Benefits = 
            {{{EffectTarget::PrimaryRate, iml}, 
              {EffectTarget::PrimaryCap, imm}, 
              {EffectTarget::ExploreRate, ims}}},
        .Penalties = 
            {{{EffectTarget::ResearchRate, dmm}, 
            {EffectTarget::PopulationRate, das}}},
        .Specialty = ResourceName::Power,
        .Weakness = ResourceName::Knowledge
    };
} // namespace

namespace Invent {
    void Society::Start(ResourceName specialty, const GameSettings& settings) {
        Specialty = specialty;
        Active = true;

        switch(specialty) {
            case ResourceName::Followers: CurrentLife = FollowerEffects.ToLife(*this, settings); break;
            case ResourceName::Knowledge: CurrentLife = KnowledgeEffects.ToLife(*this, settings); break;
            case ResourceName::Money: CurrentLife = MoneyEffects.ToLife(*this, settings); break;
            case ResourceName::Power: CurrentLife = PowerEffects.ToLife(*this, settings); break;
            default: DR_ASSERT_MSG(false, "Invalid specialty"); break;
        }

        for (const auto& effect : RebirthEffects) {
            CurrentLife.ApplyEffect(effect);
        }

        Log::Info(std::format(
            "Started society with specialty {}, primary rate {}, primary cap {} "
            "secondary rate {}, secondary cap {} Max Projects {} "
            "BuildRate Rate {}, ExploreRate Rate {}, ResearchRate Rate {}",
            ToString(specialty),
            CurrentLife.ResourceProgressions[ResourceName::Primary].GetProgress(OneSecond),
            CurrentLife.Resources[ResourceName::Primary].Capacity,
            CurrentLife.ResourceProgressions[ResourceName::Followers].GetProgress(OneSecond),
            CurrentLife.Resources[ResourceName::Followers].Capacity,
            CurrentLife.MaxWorkers,
            CurrentLife.ProjectTimeCostModifiers[ProjectType::Build].Mul,
            CurrentLife.ProjectTimeCostModifiers[ProjectType::Explore].Mul,
            CurrentLife.ProjectTimeCostModifiers[ProjectType::Research].Mul
        ));
    }

    void Society::Retire() {
        Active = false;
        // recalculate bonuses
        // start new life
    }

    void Society::Tick(BaseTime elapsed) {
        if(Active) CurrentLife.Tick(elapsed);
    }
} // namespace Invent