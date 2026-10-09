#include "Walker/WalkerRates.h"

#include <cmath>

namespace {
	using namespace Walker;


	u64 GetReward(Distance d, Distance t, f64 growth) {
		if(t <= Zero || !std::isfinite(growth) || growth <= 1.0) throw std::domain_error("Expected positive threshold and growth > 1");
		if(d < t) return 0;

		auto steps = std::max(0.0, Distance::Log10Ratio(d, t) / std::log10(growth));
		auto reward = 1.0 + std::floor(steps);
		if(!std::isfinite(reward) || reward >= std::ldexp(1.0, 64)) {
			return std::numeric_limits<u64>::max();
		}

		return static_cast<u64>(reward);
	}
}

namespace Walker {
	Quantity RateBonus::Apply(Quantity base) const {
		base *= PreMul;
		base.Pow(Exponent);
		base *= PostMul;
		return base;
	}

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

		auto result = OneMinute;
		for(size_t i = 0; i < tech.CurrentLevel && result > ZeroTime; i++) {
			result /= 2;
		}

		return result;
	}

	BaseTime WalkerRates::GetOfflineTimeCapacity() const {
		const auto& tech = m_Tech[TechKind::OfflineCapacity];
		auto bonus = tech.Researched
			? tech.CurrentLevel
			: 0;
		auto bonusTime = std::chrono::duration_cast<BaseTime>(OneHour * bonus);
		return m_MaxOfflineTime + bonusTime;
	}

	f32 WalkerRates::GetOfflineTimeEfficiency() const {
		const auto& tech = m_Tech[TechKind::OfflineEfficiency];
		auto bonus = tech.Researched
			? static_cast<f32>(tech.CurrentLevel) * 0.1f
			: 0.f;

		return std::min(1.f, m_OfflineTimeEfficiency + bonus);
	}

	u64 WalkerRates::GetRebirthReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(6), 1.5);
	}

	u64 WalkerRates::GetPrestiegeReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(10), 2.5);
	}

	u64 WalkerRates::GetAscendReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(16), 3.75);
	}

	void WalkerRates::Prestiege() {
		m_Rebirth.Reset();
	}

	void WalkerRates::Ascend() {
		m_Prestiege.Reset();
	}

	RateBonus WalkerRates::GetBonus(u64 WalkerProgression::* points) const {
		auto ascendBoost = 1.0 + static_cast<f64>(m_Ascend.*points);
		auto preMul = (1.0 + static_cast<f64>(m_Rebirth.*points)) * ascendBoost;
		auto exp = 1.0 + static_cast<f64>(m_Prestiege.*points) * 0.1 * (1.0 + static_cast<f64>(m_Ascend.*points));
		auto postMul = 1.0;
		if(points == &WalkerProgression::AccelPoints) {
			postMul += static_cast<f64>(m_Milestones.GetUnlockedTier(MilestoneKind::Travel)) * 0.1;
		}
		return {
			.PreMul = preMul,
			.Exponent = exp,
			.PostMul = postMul
		};
	}

	Quantity WalkerRates::Calculate(Quantity base, u64 WalkerProgression::* points) const {
		return GetBonus(points).Apply(base);
	}
}