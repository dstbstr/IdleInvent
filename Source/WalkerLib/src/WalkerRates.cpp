#include "Walker/WalkerRates.h"

namespace Walker {
	WorkRate WalkerRates::GetCargoWorkRate() const {
		auto rate = m_BaseCargoRate * (1 + m_Rebirth.CargoWorkPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.CargoWorkPoints) * 0.1;
		return rate.Pow(exp);
	}

	WorkRate WalkerRates::GetJobWorkRate() const {
		auto rate = m_BaseJobRate * (1 + m_Rebirth.JobWorkPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.JobWorkPoints) * 0.1;
		return rate.Pow(exp);
	}

	Acceleration WalkerRates::GetBaseAccel(VehicleKind kind) const {
		auto rate = GetVehicleDetails(kind).BaseAcceleration * (1 + m_Rebirth.AccelPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.AccelPoints) * 0.1;
		return rate.Pow(exp);
	}

	Acceleration WalkerRates::GetPoweredAccel(VehicleKind kind) const {
		auto rate = GetVehicleDetails(kind).PoweredAcceleration * (1 + m_Rebirth.AccelPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.AccelPoints) * 0.1;
		return rate.Pow(exp);
	}

	Speed WalkerRates::GetVehicleMaxSpeed(VehicleKind kind) const {
		auto rate = GetVehicleDetails(kind).MaxSpeed * (1 + m_Rebirth.MaxSpeedPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.MaxSpeedPoints) * 0.1;
		return rate.Pow(exp);
	}

	Mass WalkerRates::GetVehicleCapacity(VehicleKind kind) const {
		auto rate = GetVehicleDetails(kind).MaxCapacity * (1 + m_Rebirth.MaxCapacityPoints);
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.MaxCapacityPoints) * 0.1;
		return rate.Pow(exp);
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

	void WalkerRates::Prestiege() {
		m_Rebirth.Reset();
	}

	void WalkerRates::Ascend() {
		m_Prestiege.Reset();
	}
}