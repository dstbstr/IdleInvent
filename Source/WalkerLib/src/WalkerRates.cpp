#include "Walker/WalkerRates.h"

namespace Walker {
	WorkRate WalkerRates::GetCargoWorkRate() const {
		return Calculate(m_BaseCargoRate, &WalkerProgression::CargoWorkPoints);
	}

	WorkRate WalkerRates::GetJobWorkRate() const {
		return Calculate(m_BaseJobRate, &WalkerProgression::JobWorkPoints);
	}

	Acceleration WalkerRates::GetBaseAccel(VehicleKind kind) const {
		return Calculate(GetVehicleDetails(kind).BaseAcceleration, &WalkerProgression::AccelPoints);
	}

	Acceleration WalkerRates::GetPoweredAccel(VehicleKind kind) const {
		return Calculate(GetVehicleDetails(kind).PoweredAcceleration, &WalkerProgression::AccelPoints);
	}

	Speed WalkerRates::GetVehicleMaxSpeed(VehicleKind kind) const {
		return Calculate(GetVehicleDetails(kind).MaxSpeed, &WalkerProgression::MaxSpeedPoints);
	}

	Mass WalkerRates::GetVehicleCapacity(VehicleKind kind) const {
		return Calculate(GetVehicleDetails(kind).MaxCapacity, &WalkerProgression::MaxCapacityPoints);
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

	std::pair<Quantity, double> WalkerRates::GetBonus(u64 WalkerProgression::* points) const {
		auto ascendBoost = Quantity{ m_Ascend.*points} + 1;
		auto mul = (Quantity{ m_Rebirth.*points } + 1) * ascendBoost;
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.*points) * 0.1 * (1.0 + static_cast<f64>(m_Ascend.*points));

		return {mul, exp};
	}

	Quantity WalkerRates::Calculate(Quantity base, u64 WalkerProgression::* points) const {
		auto [mul, exp] = GetBonus(points);
		auto result = base * mul;
		return result.Pow(exp);
	}
}