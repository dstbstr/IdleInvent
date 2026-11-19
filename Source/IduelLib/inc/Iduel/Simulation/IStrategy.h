#pragma once

#include "Iduel/Economy/Generator.h"

#include <optional>
#include <variant>

namespace Iduel {
    struct EconomyState;
    struct ParticipantState;

    struct MinimizeTime {
        f64 Target;
    };
    struct MaximizeIncome {
        BaseTime Duration;
    };
    struct MaximizeWealth {
		BaseTime Duration;
	};

	using Objective = std::variant<MinimizeTime, MaximizeIncome, MaximizeWealth>;
    struct StrategyContext {
        Objective Goal;
        BaseTime Elapsed;
    };

    struct Purchase {
        size_t GeneratorIndex{};
        size_t Count{ 1 };
    };

    struct IStrategy {
        virtual ~IStrategy() = default;
        IStrategy() = default;
        IStrategy(const IStrategy&) = delete;

        virtual std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) = 0;
    };

    struct NoPurchasesStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) override {
			return std::nullopt;
		}
    };

    struct GreedyStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) override;
	};

    struct SingleLookaheadStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) override;
    };

	struct RandomStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant, const StrategyContext& context) override;

    private:
        size_t m_NextGenIndex{0};
	};
}