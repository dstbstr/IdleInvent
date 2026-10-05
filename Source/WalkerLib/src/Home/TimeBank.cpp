#include "Walker/Home/TimeBank.h"

#include <Instrumentation/Logging.h>

#include <algorithm>

namespace Walker {
	TimeBank::TimeBank(const WalkerRates& rates) : m_Rates(rates) {}

	void TimeBank::AddTime(BaseTime time) {
		DR_ASSERT_MSG(time > ZeroTime, "Time must be greater than zero");
		if(time > ZeroTime) {
			auto efficiency = static_cast<f64>(m_Rates.GetOfflineTimeEfficiency());
			auto effectiveTime = std::chrono::duration_cast<BaseTime>(efficiency * time);

			auto capacity = m_Rates.GetOfflineTimeCapacity();
			auto available = std::max(ZeroTime, capacity - m_Time);
			m_Time += std::min(available, effectiveTime);
		}
	}

	void TimeBank::SetRate(u64 rate) {
		DR_ASSERT_MSG(rate > 0, "Rate must be greater than zero");
		if(rate > 0) {
			m_Rate = rate;
		}
	}

	BaseTime TimeBank::Spend(BaseTime elapsed) {
		if (elapsed <= ZeroTime || m_Rate <= 1 || !m_Spending) return elapsed;
		
		auto mul = static_cast<BaseTime::rep>(m_Rate - 1);
		auto extra = std::min(m_Time, elapsed * mul);
		m_Time -= extra;

		return elapsed + extra;
	}
}