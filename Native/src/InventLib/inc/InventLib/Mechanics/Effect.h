#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Resources/Resource.h"

#include <functional>

namespace Invent {
	struct Effect {
		ResourceName Resource;
        BaseTime Duration;

		s64 Add{ 0 };
		f32 Mul{ 1.0f };
		std::function<void()> OtherEffects{};
	};

	namespace Effects {
		Effect Create(ResourceName resource, s64 add, BaseTime duration);
		Effect Create(ResourceName resource, f32 mul, BaseTime duration);
		Effect Create(std::function<void()> effect, BaseTime duration);

		namespace Buff {
			namespace Add {
				constexpr s64 None = 0;
				constexpr s64 Small = 1;
				constexpr s64 Medium = 3;
				constexpr s64 Large = 5;
				constexpr s64 XLarge = 10;
				constexpr s64 XXLarge = 25;
			}
			namespace Mul {
				constexpr f32 None = 1.0f;
				constexpr f32 Small = 1.1f;
				constexpr f32 Medium = 1.3f;
				constexpr f32 Large = 1.5f;
				constexpr f32 XLarge = 2.f;
				constexpr f32 XXLarge = 3.f;
			}

			constexpr s64 Next(s64 current) {
				switch (current) {
				case Buff::Add::None: return Buff::Add::Small;
				case Buff::Add::Small: return Buff::Add::Medium;
				case Buff::Add::Medium: return Buff::Add::Large;
				case Buff::Add::Large: return Buff::Add::XLarge;
				case Buff::Add::XLarge: return Buff::Add::XXLarge;
				default: return static_cast<s64>(current * 1.5);
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
				constexpr s64 Small = -1;
				constexpr s64 Medium = -2;
				constexpr s64 Large = -5;
				constexpr s64 XLarge = -10;
			}
			namespace Mul {
				constexpr f32 Small = -0.3f;
				constexpr f32 Medium = -0.5f;
				constexpr f32 Large = -1.0f;
				constexpr f32 XLarge = -2.0f;
			}
		}
	}
}