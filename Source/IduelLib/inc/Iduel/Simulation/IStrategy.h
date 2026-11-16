#pragma once

#include "Iduel/Economy/Generator.h"

#include <optional>

namespace Iduel {
    struct EconomyState;
    struct ParticipantState;

    struct Purchase {
        size_t GeneratorIndex{};
        size_t Count{ 1 };
    };

    struct IStrategy {
        virtual ~IStrategy() = default;
        IStrategy() = default;
        IStrategy(const IStrategy&) = delete;

        virtual std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant) = 0;
    };

    struct NoPurchasesStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant) override {
			return std::nullopt;
		}
    };

    struct GreedyStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant) override;
	};

	struct RandomStrategy : public IStrategy {
		std::optional<Purchase> ChoosePurchase(const EconomyState& economy, const ParticipantState& participant) override;

    private:
        size_t m_NextGenIndex{0};
	};
}