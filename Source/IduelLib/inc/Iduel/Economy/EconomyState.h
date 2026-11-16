#pragma once

#include "Iduel/Economy/Generator.h"
#include "GameState/GameTime.h"

#include <span>
#include <vector>

namespace Iduel {
	struct EconomyState {
		std::vector<size_t> GenCounts{};
		std::span<const Generator> Generators{};
		std::vector<BaseTime> PayoutElapsed{};
	};
}