#include "Walker/WalkerRates.h"

namespace Walker {
	WorkRate WalkerRates::GetCargoWorkRate() const {
		return m_BaseCargoRate * m_Rebirth.CargoWorkRateMultiplier;
	}

	WorkRate WalkerRates::GetJobWorkRate() const {
		return m_BaseJobRate * m_Rebirth.JobWorkRateMultiplier;
	}

	Speed WalkerRates::GetVehicleMaxSpeed(VehicleKind kind) const {
		return GetVehicleDetails(kind).MaxSpeed * m_Rebirth.MaxSpeedMultiplier;
	}

	Mass WalkerRates::GetVehicleCapacity(VehicleKind kind) const {
		return GetVehicleDetails(kind).MaxCapacity * m_Rebirth.MaxCapacityMultiplier;
	}

	Money WalkerRates::GetHireCost(u64 hiredCount, u64 count) const {
		Money total{};
		for(u64 i = 0; i < count; i++) {
			auto cost = m_BaseHireCost;
			cost.ScaleByPower(m_HireCostBase, hiredCount + i);
			total += cost;
		}
		return total;
	}

	u64 WalkerRates::GetMaxHireCount(Money funds, u64 hiredCount) const {
		u64 count = 0;
		Money total{};
		while(true) {
			auto nextTotal = total + GetHireCost(hiredCount + count, 1);
			if(nextTotal > funds) return count;
			total = nextTotal;
			count++;
		}
	}

	std::optional<BaseTime> WalkerRates::GetPreparationDuration() const {
		const auto& tech = m_Tech[TechKind::Preparation];
		if (!tech.Researched) return std::nullopt;

		auto result = OneSecond * 30;
		for(size_t i = 0; i < tech.CurrentLevel && result > ZeroTime; i++) {
			result /= 2;
		}

		return result;
	}

}