#include "Iduel/Economy/Generator.h"

namespace Iduel {
	f64 Generator::GetIncomePerPayout(s32 owned) const {
		auto rate = Income.Evaluate(static_cast<f64>(owned));
		for (const auto& threshold : Thresholds) {
			if (static_cast<f64>(owned) >= threshold.Index) {
				rate *= threshold.IncomeMultiplier;
			}
		}
		return rate;
	}

	BaseTime Generator::GetTimePerPayout(s32 owned) const {
		auto rate = TimePerIncome;
		for (const auto& threshold : Thresholds) {
			if (static_cast<f64>(owned) >= threshold.Index) {
				rate /= threshold.SpeedMultiplier;
			}
		}
		return rate;
	}

	f64 Generator::GetCost(s32 owned) const {
		return Cost.Evaluate(static_cast<f64>(owned));
	}
}