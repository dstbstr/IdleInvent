#pragma once

#include "Core/NumTypes.h"

#include <vector>
#include <numeric>

namespace Invent {
	struct Progression {
		s64 BaseProgress{ 1 };
		std::vector<s64> AddMods{};
		std::vector<f32> MulMods{};

		constexpr s64 GetProgress() const {
			auto addBonus = std::accumulate(AddMods.begin(), AddMods.end(), 0ll, std::plus<s64>());
			auto mulBonus = std::accumulate(MulMods.begin(), MulMods.end(), 1.0f, std::multiplies<f32>());
			return static_cast<s64>((BaseProgress + addBonus) * mulBonus);
		}

		constexpr void Reset() {
			AddMods.clear();
			MulMods.clear();
		}
	};
}