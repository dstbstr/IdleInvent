#pragma once

#include "Walker/WalkerRates.h"
#include <Platform/NumTypes.h>
#include <GameState/GameTime.h>

namespace Walker {
	class TimeBank {
	public:
		TimeBank(const WalkerRates& rates);

		BaseTime GetRemaining() const { return m_Time; }
		void AddTime(BaseTime time);
		void SetRate(u64 rate);
		u64 GetRate() const { return m_Rate; }
		bool IsSpending() const { return m_Spending; }
		void SetSpending(bool enabled) { m_Spending = enabled; }

		BaseTime Spend(BaseTime elapsed);
	private:
		const WalkerRates& m_Rates;
		BaseTime m_Time{};
		u64 m_Rate{1};
		bool m_Spending{false};
	};
}