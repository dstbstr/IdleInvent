#pragma once

#include "Iduel/Curve.h"
#include "GameState/GameTime.h"

namespace Iduel {
	struct Threshold {
		s32 Index{0};
		f64 IncomeMultiplier{1.0};
		f64 SpeedMultiplier{1.0};
	};

	struct Generator {
		Curve Income{};
		Curve Cost{};
		BaseTime TimePerIncome{OneSecond};
		std::vector<Threshold> Thresholds{};

		std::string Name{};

		f64 GetIncomePerPayout(s32 owned) const;
		BaseTime GetTimePerPayout(s32 owned) const;
		f64 GetCost(s32 owned) const;
	};
}