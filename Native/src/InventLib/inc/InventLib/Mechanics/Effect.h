#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Resources/Resource.h"

#include <functional>

namespace Invent {
	enum struct EffectTarget {
		Unset,
		Resources,
		Storages
	};

	struct Effect {
		ResourceName Resource;
		EffectTarget Target;
        BaseTime Duration;

		s64 Add{ 0 };
		f32 Mul{ 1.0f };
		std::function<void()> OtherEffects{};
	};

	namespace Effects {
		Effect Create(ResourceName resource, EffectTarget target, s64 add, BaseTime duration);
		Effect Create(ResourceName resource, EffectTarget target, f32 mul, BaseTime duration);
		Effect Create(std::function<void()> effect, BaseTime duration);

		namespace Buff {
			namespace Add {
				constexpr s64 None = 0;
				constexpr s64 Small = 10;
				constexpr s64 Medium = 100;
				constexpr s64 Large = 1000;
				constexpr s64 XLarge = 10000;
				constexpr s64 XXLarge = 100000;
			}
			namespace Mul {
				constexpr f32 None = 1.0f;
				constexpr f32 Small = 1.1f;
				constexpr f32 Medium = 1.5f;
				constexpr f32 Large = 2.0f;
				constexpr f32 XLarge = 3.0f;
				constexpr f32 XXLarge = 10.0f;
			}

			constexpr s64 Next(s64 current) {
				switch (current) {
				case Buff::Add::None: return Buff::Add::Small;
				case Buff::Add::Small: return Buff::Add::Medium;
				case Buff::Add::Medium: return Buff::Add::Large;
				case Buff::Add::Large: return Buff::Add::XLarge;
				case Buff::Add::XLarge: return Buff::Add::XXLarge;
				default: return current * 10;
				}
			}
			constexpr f32 Next(f32 current) {
				if (current == Buff::Mul::None) return Buff::Mul::Small;
				if (current == Buff::Mul::Small) return Buff::Mul::Medium;
				if (current == Buff::Mul::Medium) return Buff::Mul::Large;
				if (current == Buff::Mul::Large) return Buff::Mul::XLarge;
				if (current == Buff::Mul::XLarge) return Buff::Mul::XXLarge;
				return current * 2.0f;
			}
		}

		namespace Penalty {
			namespace Add {
				constexpr s64 Small = -10;
				constexpr s64 Medium = -100;
				constexpr s64 Large = -1000;
				constexpr s64 XLarge = -10000;
			}
			namespace Mul {
				constexpr f32 Small = 0.9f;
				constexpr f32 Medium = 0.5f;
				constexpr f32 Large = 0.1f;
				constexpr f32 XLarge = 0.0f;
			}
		}
	}
}