#include "Walker/WalkerRates.h"

#include <array>
#include <cmath>
#include <limits>
#include <optional>

namespace {
	using namespace Walker;

	struct UpgradeCostDetails {
		ProgressionPoints InitialCost;
		f64 Growth;
	};

	static const auto UpgradeCosts = []{
		std::array<std::optional<UpgradeCostDetails>, UpgradeKey::SlotCount> result{};
		auto Global = [&](GlobalStat stat, ProgressionPoints initial, f64 growth) {
			result.at(UpgradeKey::Global(stat).GetIndex()) = UpgradeCostDetails{ initial, growth };
		};
		auto Vehicle = [&](VehicleKind kind, VehicleStat stat, ProgressionPoints initial, f64 growth) {
			result.at(UpgradeKey::Vehicle(kind, stat).GetIndex()) = UpgradeCostDetails{ initial, growth };
		};
		auto Crew = [&](CrewRole role, ProgressionPoints initial, f64 growth) {
			result.at(UpgradeKey::Job(role).GetIndex()) = UpgradeCostDetails{ initial, growth };
		};
		Global(GlobalStat::CargoWork, 1, 1.2);
		Global(GlobalStat::Jobs, 1, 1.2);
		Global(GlobalStat::Accel, 1, 1.2);
		Global(GlobalStat::Speed, 1, 1.2);
		Global(GlobalStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Foot, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Foot, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Foot, VehicleStat::Capacity, 1, 1.2);
		
		Vehicle(VehicleKind::Bike, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Bike, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Bike, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Car, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Car, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Car, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Boat, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Boat, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Boat, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Plane, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Plane, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Plane, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Jet, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Jet, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Jet, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Rocket, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Rocket, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Rocket, VehicleStat::Capacity, 1, 1.2);

		Vehicle(VehicleKind::Starship, VehicleStat::Accel, 1, 1.2);
		Vehicle(VehicleKind::Starship, VehicleStat::Speed, 1, 1.2);
		Vehicle(VehicleKind::Starship, VehicleStat::Capacity, 1, 1.2);

		Crew(CrewRole::Engineer, 1, 1.2);
		Crew(CrewRole::Scout, 1, 1.2);
		Crew(CrewRole::Scientist, 1, 1.2);

		return result;
	}();

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
	WalkerRates::WalkerRates(const TechManager& tech, const MilestoneManager& milestones) 
		: m_Tech(tech)
		, m_Milestones(milestones) {}

	Quantity RateBonus::Apply(Quantity base) const {
		base *= PreMul;
		base.Pow(Exponent);
		base *= PostMul;
		return base;
	}

	WorkRate WalkerRates::GetCargoWorkRate() const {
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::CargoWork));
		return global.Apply(m_BaseCargoRate);
	}

	WorkRate WalkerRates::GetJobWorkRate(CrewRole role) const {
		auto base = m_BaseJobRate;
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::Jobs));
		auto local = GetBonus(UpgradeKey::Job(role));
		return global.Combine(local).Apply(base);
	}

	Acceleration WalkerRates::GetBaseAccel(VehicleKind kind) const {
		auto base = GetVehicleDetails(kind).BaseAcceleration;
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::Accel));
		auto local = GetBonus(UpgradeKey::Vehicle(kind, VehicleStat::Accel));
		return global.Combine(local).Apply(base);
	}

	Acceleration WalkerRates::GetPoweredAccel(VehicleKind kind) const {
		auto base = GetVehicleDetails(kind).PoweredAcceleration;
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::Accel));
		auto local = GetBonus(UpgradeKey::Vehicle(kind, VehicleStat::Accel));
		return global.Combine(local).Apply(base);
	}

	Speed WalkerRates::GetVehicleMaxSpeed(VehicleKind kind) const {
		auto base = GetVehicleDetails(kind).MaxSpeed;
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::Speed));
		auto local = GetBonus(UpgradeKey::Vehicle(kind, VehicleStat::Speed));
		return global.Combine(local).Apply(base);
	}

	Mass WalkerRates::GetVehicleCapacity(VehicleKind kind) const {
		auto base = GetVehicleDetails(kind).MaxCapacity;
		auto global = GetBonus(UpgradeKey::Global(GlobalStat::Capacity));
		auto local = GetBonus(UpgradeKey::Vehicle(kind, VehicleStat::Capacity));
		return global.Combine(local).Apply(base);
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

	Money WalkerRates::GetCargoValue(Mass amount, EndpointKind kind) const {
		auto kindValue = static_cast<u8>(kind);
		return m_CargoValue * amount * kindValue;
	}

	Money WalkerRates::GetVehicleCost(VehicleKind kind) const {
		auto baseCost = GetVehicleDetails(kind).Cost;
		// TODO: Balance as needed
		return baseCost;
	}

	Work WalkerRates::GetScoutingWork(EndpointKind kind, size_t completions) const {
		auto dest = static_cast<size_t>(kind);
		auto baseWork = Work{ 10 }.Pow(dest) * (completions + 1);
		// TODO: Balance as needed
		return baseWork;
	}

	Work WalkerRates::GetResearchWork(TechKind kind) const {
		auto baseWork = GetBaseScienceWork(kind);
		// TODO: Balance as needed
		return baseWork;
	}

	Work WalkerRates::GetEngineeringWork(TechKind kind, u64 currentLevel) const {
		auto baseWork = GetBaseEngineeringWork(kind, currentLevel);
		// TODO: Balance as needed
		return baseWork;
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

	ProgressionPoints WalkerRates::GetNextLevelCost(const UpgradeKey& key, UpgradeLevel currentLevel) const {
		auto details = UpgradeCosts.at(key.GetIndex());
		DR_ASSERT(details);
		if (!details) return std::numeric_limits<ProgressionPoints>::max();

		auto [base, growth] = *details;
		auto cost = std::ceil(static_cast<f64>(base) * std::pow(growth, currentLevel));

		auto max = std::ldexp(1.0, std::numeric_limits<ProgressionPoints>::digits);
		if(!std::isfinite(cost) || cost >= max) {
			return std::numeric_limits<ProgressionPoints>::max();
		}
		return static_cast<ProgressionPoints>(cost);
	}

	u64 WalkerRates::GetRebirthReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(6), 1.5);
	}

	u64 WalkerRates::GetPrestigeReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(10), 2.5);
	}

	u64 WalkerRates::GetAscendReward(Distance distance) const {
		return GetReward(distance, Distance::Pow10(16), 3.75);
	}

	void WalkerRates::PrestigeReset() {
		m_Rebirth.Reset();
	}

	void WalkerRates::AscendReset() {
		m_Prestige.Reset();
	}

	RateBonus WalkerRates::GetBonus(UpgradeKey key) const {
		auto rebirth = static_cast<f64>(m_Rebirth.GetLevel(key));
		auto Prestige = static_cast<f64>(m_Prestige.GetLevel(key));
		auto ascend = static_cast<f64>(m_Ascend.GetLevel(key));
		ascend += 1.0;
		return {
			.PreMul = (1.0 + rebirth) * ascend,
			.Exponent = 1.0 + Prestige * 0.1 * ascend
		};
	}
}