#pragma once

#include "Iduel/IduelTypes.h"

#include <GameState/GameTime.h>

namespace Iduel {
	struct Level {
		static constexpr Quantity UnitsPerWhole = Quantity{10}.Pow(11);
		Money BaseCost{};
		f64 CostScale{1.0};
		u32 PurchasedCount{};
		u32 PowerCount{};
		Quantity TotalCount{};
		Quantity OutputPerSecond{};

		Money GetPurchaseCost() const {
			auto cost = BaseCost;
			return cost.ScaleByPower(CostScale, PurchasedCount);
		}

		Money GetPowerCost() const {
			auto cost = BaseCost;
			return cost.ScaleByPower(CostScale, PowerCount * 4);
		}

		Quantity GetProduction(Quantity ms) const {
			auto scale = Level::UnitsPerWhole * Quantity{1000};
			auto produced = TotalCount * OutputPerSecond * ms;
			return produced / scale;
		}

		bool CanPurchase(Money balance) const {
			return balance >= GetPurchaseCost();
		}

		bool TryPurchase(Money& balance) {
			auto cost = GetPurchaseCost();
			if(cost > balance) return false;

			balance -= cost;
			PurchasedCount++;
            TotalCount += UnitsPerWhole;
			return true;
		}

		bool CanEmpower(Money balance) const { return balance >= GetPowerCost(); }
        bool TryEmpower(Money& balance) {
            auto cost = GetPowerCost();
            if(cost > balance) return false;
            balance -= cost;
            PowerCount++;
            return true;
        }
	};

	class Economy {
    public:
		Economy() {
			m_Levels.resize(10);
			for(size_t i = 0; i < m_Levels.size(); i++) {
                m_Levels[i] = {
					.BaseCost = Money{1'000} * Money{10}.Pow(i + 1),
					.CostScale = 1.15 + (.1 * i),
					.OutputRate = i == 0 
						? 1.0 
						: 0.2 * std::pow(0.1, static_cast<f64>(i - 1)),
					.PurchasedCount = 0,
					.PowerCount = 1,
					.TotalCount = 0,
				};
			}
		}

		Money GetBalance() const { return m_Balance; }
		
		void Tap() {
			m_Balance += 1;
		}

		void Tick(BaseTime elapsed) {
			if(elapsed <= ZeroTime || m_Levels.empty()) return;

			auto seconds = static_cast<f64>(elapsed.count()) / static_cast<f64>(OneSecond.count());

			auto investorExpense = Money{m_Investors} * m_InvestorExpense * seconds;
            auto investorRate = 0.1 * m_Investors;
            auto investorIncome = m_Balance * investorRate * seconds;

            if(m_Balance >= investorExpense) {
                m_Balance -= investorExpense;
                m_Balance += investorIncome;
            }

			std::vector<Quantity> production(m_Levels.size(), Zero);

			for(size_t source = 1; source < m_Levels.size(); ++source) {
                const auto& level = m_Levels[source];
				production[source - 1] += level.GetProduction(seconds);
			}
            auto money = m_Levels[0].TotalCount * (1'000 * seconds) * m_Levels[0].PowerCount;
			m_Balance += money;
			for(size_t level = 0; level < m_Levels.size(); ++level) {
				m_Levels[level].TotalCount += production[level];
			}

		}

		std::vector<Level> GetLevels() const { return m_Levels; }

		Level GetLevel(size_t index) const {
			DR_ASSERT(index < m_Levels.size());
            return m_Levels[index];
		}

		bool CanBuyLevel(size_t level) const {
			DR_ASSERT(level < m_Levels.size());
            return m_Levels[level].CanPurchase(m_Balance);
		}

        bool TryBuyLevel(size_t level) {
            DR_ASSERT(level < m_Levels.size());
			return m_Levels[level].TryPurchase(m_Balance);
        }

		bool CanEmpowerLevel(size_t level) const {
            DR_ASSERT(level < m_Levels.size());
            return m_Levels[level].CanEmpower(m_Balance);
        }
        bool TryEmpowerLevel(size_t level) {
            DR_ASSERT(level < m_Levels.size());
            return m_Levels[level].TryEmpower(m_Balance);
        }

		Money GetInvestorCost() const { return (1 + m_Investors) * m_InvestorCost; }
        bool CanBuyInvestor() const { return m_Balance >= GetInvestorCost(); }
		bool TryBuyInvestor() {
            auto cost = GetInvestorCost();
            if(cost > m_Balance) return false;
            m_Balance -= cost;
            m_Investors++;
            return true;
        }
        size_t GetInvestorCount() const { return m_Investors; }

	private:
		Money m_Balance{};
		std::vector<Level> m_Levels{};

		size_t m_Investors{0};

		Money m_InvestorExpense{1'000'000};
		Money m_InvestorCost{1'000'000'000};
	};
}