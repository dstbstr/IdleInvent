#include "Iduel/Simulation/IStrategy.h"
#include "Iduel/Economy/EconomyState.h"
#include "Iduel/Simulation/ParticipantState.h"

namespace {
	using namespace Iduel;
	size_t GetAffordable(const Generator& gen, size_t owned, f64 wallet) {
		size_t count = 0;
		auto cost = gen.GetCost(owned + count);
		while (wallet >= cost) {
			wallet -= cost;
			count++;
			cost = gen.GetCost(owned + count);
		}

		return count;
	}

	std::optional<Purchase> SolveMaxIncome(const EconomyState& economy, const ParticipantState& participant, BaseTime elapsedTime, BaseTime totalTime, f64 currentIncome) {
		return std::nullopt;
	}

	std::optional<Purchase> SolveMaxWealth(const EconomyState& economy, const ParticipantState& participant, BaseTime elapsedTime, BaseTime totalTime, f64 currentIncome) {
		auto remainingTime = totalTime - elapsedTime;
		auto endingWealth = participant.Wallet;
		auto incomePerSecond = currentIncome / static_cast<f64>(OneSecond.count());
		auto remainingSeconds = static_cast<f64>(remainingTime.count()) / static_cast<f64>(OneSecond.count());
		endingWealth += incomePerSecond * remainingSeconds;

		return std::nullopt;
	}

	std::optional<Purchase> SolveMinTime(const EconomyState& economy, const ParticipantState& participant, BaseTime elapsedTime, f64 target, f64 currentIncome) {
		return std::nullopt;
	}
}

namespace Iduel {
	std::optional<Purchase> SingleLookaheadStrategy::ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) {
		f64 currentIncome = 0.0;
		for (size_t i = 0; i < economy.Generators.size(); i++) {
			auto& gen = economy.Generators[i];
			auto owned = economy.GenCounts[i];
			if(owned == 0) continue;

			auto seconds = static_cast<f64>(gen.GetTimePerPayout(owned).count()) / static_cast<f64>(OneSecond.count());
			currentIncome += gen.GetIncomePerPayout(owned) / seconds;
		}

		return std::visit([&](auto&& arg) -> std::optional<Purchase> {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, MinimizeTime>) {
				return SolveMinTime(economy, participant, arg.Target, currentIncome);
			}
			else if constexpr (std::is_same_v<T, MaximizeIncome>) {
				return SolveMaxIncome(economy, participant, arg.Duration, currentIncome);
			}
			else if constexpr (std::is_same_v<T, MaximizeWealth>) {
				return SolveMaxWealth(economy, participant, arg.Duration, currentIncome);
			}
			else {
				static_assert(always_false<T>::value, "Non-exhaustive visitor!");
			}
			}, context.Goal);
	}

}