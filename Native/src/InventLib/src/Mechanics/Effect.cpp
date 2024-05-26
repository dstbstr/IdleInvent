#include "InventLib/Mechanics/Effect.h"

namespace Invent {
	namespace Effects {
		Effect CreateTemp(ResourceName resource, EffectTarget target, s64 add) {
			return Effect{
				.Sentiment = add > 0 ? EffectSentiment::Positive : EffectSentiment::Negative,
				.Resource = resource,
				.Target = target,
				.Permanent = false,
				.Add = add
			};
		}

		Effect CreateTemp(ResourceName resource, EffectTarget target, f32 mul) {
			return Effect{
				.Sentiment = mul > 1.0f ? EffectSentiment::Positive : EffectSentiment::Negative,
				.Resource = resource,
				.Target = target,
				.Permanent = false,
				.Mul = mul
			};
		}

		Effect CreateTemp(std::function<void()> effect, EffectSentiment sentiment) {
			return Effect{
				.Sentiment = sentiment,
				.Resource = ResourceName::Unset,
				.Target = EffectTarget::Resources, // not really though
				.Permanent = false,
				.OtherEffects = effect
			};
		}

		Effect CreatePermanent(ResourceName resource, EffectTarget target, s64 add) {
			return Effect{
				.Sentiment = add > 0 ? EffectSentiment::Positive : EffectSentiment::Negative,
				.Resource = resource,
				.Target = target,
				.Permanent = true,
				.Add = add
			};
		}

		Effect CreatePermanent(ResourceName resource, EffectTarget target, f32 mul) {
			return Effect{
				.Sentiment = mul > 1.0f ? EffectSentiment::Positive : EffectSentiment::Negative,
				.Resource = resource,
				.Target = target,
				.Permanent = true,
				.Mul = mul
			};
		}

		Effect CreatePermanent(std::function<void()> effect, EffectSentiment sentiment) {
			return Effect{
				.Sentiment = sentiment,
				.Resource = ResourceName::Unset,
				.Target = EffectTarget::Resources, // not really though
				.Permanent = true,
				.OtherEffects = effect
			};
		}
	}

}