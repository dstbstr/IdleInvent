#include "Pets/Character/Ability.h"
#include "Pets/Pets/PetDetails.h"

#include <cmath>

namespace {
    u32 Add(u32 a, f32 b) {
        return a + static_cast<u32>(std::floor(b));
    }
    u32 Mul(u32 a, f32 b) {
        return static_cast<u32>(std::floor(static_cast<f32>(a) * b));
    }

    u32 ApplyStat(u32 stat, const Pets::Ability& ability) {
        return ability.Effect == Pets::AbilityEffect::Add ? Add(stat, ability.Value) : Mul(stat, ability.Value);
    }
    f32 ApplyStat(f32 stat, const Pets::Ability& ability) {
        return ability.Effect == Pets::AbilityEffect::Add 
            ? stat + ability.Value 
            : stat * ability.Value;
    }
}

namespace Pets {
    void ApplyAbility(const Ability& ability, PetDetails& target, f32& actionRate) {
        switch(ability.Kind) {
            using enum AbilityKind;
            case Attack: target.Attack = ApplyStat(target.Attack, ability); break;
            case Piercing: target.Piercing = ApplyStat(target.Piercing, ability); break;
            case Capacity: target.PetCapacity = ApplyStat(target.PetCapacity, ability); break;
            case Ability: {
                for(auto& a : target.Abilities) {
                    a.Value.Value = ApplyStat(a.Value.Value, ability);
                }
            }
            break;
            case AbilityKind::ActionRate: actionRate = ApplyStat(actionRate, ability); break;
            case Mana: break; // TODO
        }    
    }
}