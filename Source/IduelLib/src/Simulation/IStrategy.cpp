#include "Iduel/Simulation/IStrategy.h"
#include "Iduel/Economy/EconomyState.h"
#include "Iduel/Simulation/ParticipantState.h"

#include "Utilities/IRandom.h"
#include "DesignPatterns/ServiceLocator.h"

namespace {
	using namespace Iduel;

	size_t GetAffordable(const Generator& gen, size_t owned, f64 wallet) {
		size_t count = 0;
		auto cost = gen.GetCost(owned + count);
		while(wallet >= cost) {
			wallet -= cost;
			count++;
			cost = gen.GetCost(owned + count);
		}

		return count;
	}
}

namespace Iduel {
	std::optional<Purchase> GreedyStrategy::ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) {
		for(size_t i = economy.Generators.size(); i-- > 0;) {
			auto& gen = economy.Generators[i];
			auto count = economy.GenCounts[i];
			auto wallet = participant.Wallet;
			auto buyCount = GetAffordable(gen, count, wallet);
			if (buyCount > 0) {
				return Purchase{ .GeneratorIndex = i, .Count = buyCount};
			}
		}
		return std::nullopt;
	}
	
	std::optional<Purchase> RandomStrategy::ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) {
		static auto& random = [] -> IRandom& {
			auto& services = ServiceLocator::Get();
			if(!services.IsSet<IRandom>()) {
				services.SetThisAsThat<DefaultRandom, IRandom>();
			}
			return services.GetRequired<IRandom>();
		}();

		auto& gen = economy.Generators[m_NextGenIndex];
		auto count = economy.GenCounts[m_NextGenIndex];
		if(gen.GetCost(count) <= participant.Wallet) {
			auto result = Purchase{ .GeneratorIndex = m_NextGenIndex, .Count = 1 };
			m_NextGenIndex = random.GetNextU(0, economy.Generators.size());
			return result;
		}
		return std::nullopt;
	}
}