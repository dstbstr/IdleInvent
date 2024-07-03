#include "InventLib/Mechanics/Effect.h"

namespace Invent {
	namespace Effects {
		Effect Create(ResourceName resource, s64 add, BaseTime duration) {
			return Effect{
				.Resource = resource,
				.Duration = duration,
				.Add = add
			};
		}

		Effect Create(ResourceName resource, f32 mul, BaseTime duration) {
			return Effect{
				.Resource = resource,
				.Duration = duration,
				.Mul = mul
			};
		}

		Effect Create(std::function<void()> effect, BaseTime duration) {
			return Effect{
				.Resource = ResourceName::Unset,
                .Duration = duration,
				.OtherEffects = effect
			};
		}
	}

}