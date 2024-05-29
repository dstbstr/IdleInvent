#include "InventLib/Mechanics/Effect.h"

namespace Invent {
	namespace Effects {
		Effect Create(ResourceName resource, EffectTarget target, s64 add, BaseTime duration) {
			return Effect{
				.Resource = resource,
				.Target = target, 
				.Duration = duration,
				.Add = add
			};
		}

		Effect Create(ResourceName resource, EffectTarget target, f32 mul, BaseTime duration) {
			return Effect{
				.Resource = resource,
				.Target = target, 
				.Duration = duration,
				.Mul = mul
			};
		}

		Effect Create(std::function<void()> effect, BaseTime duration) {
			return Effect{
				.Resource = ResourceName::Unset,
				.Target = EffectTarget::Unset,
                .Duration = duration,
				.OtherEffects = effect
			};
		}
	}

}