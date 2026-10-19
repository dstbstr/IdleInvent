#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Crew/CrewRole.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Milestones/Milestones.h"
#include "Walker/Rebirth/WalkerProgression.h"
#include "Walker/Travel/Vehicle.h"

#include <optional>

namespace Walker {
	// ((Base * PreMul) ^ Exponent) * PostMul
	struct RateBonus {
		f64 PreMul{ 1 };
		f64 Exponent{ 1.0 };
		f64 PostMul{ 1 };

		Quantity Apply(Quantity base) const;
		constexpr RateBonus Combine(const RateBonus& other) const {
			return {
				.PreMul = PreMul * other.PreMul,
				.Exponent = Exponent + other.Exponent - 1.0,
				.PostMul = PostMul * other.PostMul
			};
		}
	};

	class WalkerRates {
	public:
		WalkerRates(const TechManager& tech, const MilestoneManager& milestones);

		// Bonuses
		WorkRate GetCargoWorkRate() const;
		WorkRate GetJobWorkRate(CrewRole role) const;
		WorkRate GetDiscoveryWorkRate() const;
		Acceleration GetBaseAccel(VehicleKind kind) const;
		Acceleration GetPoweredAccel(VehicleKind kind) const;
		Acceleration GetDrag(VehicleKind kind, Speed speed) const;
		Time GetDragResponseTime(VehicleKind kind) const;
		Mass GetVehicleCapacity(VehicleKind kind) const;
		Money GetCargoValue(Mass amount, EndpointKind kind) const;
		std::optional<BaseTime> GetPreparationDuration() const;
		BaseTime GetOfflineTimeCapacity() const;
		f32 GetOfflineTimeEfficiency() const;

		// Costs
		Money GetVehicleCost(VehicleKind kind) const;
		Work GetScoutingWork(EndpointKind kind, size_t completions) const;
		Work GetResearchWork(TechKind kind) const;
		Work GetEngineeringWork(TechKind kind, u64 currentLevel) const;
		Money GetHireCost(u64 hiredCount, u64 count = 1) const;
		u64 GetMaxHireCount(Money funds, u64 hiredCount) const;

		ProgressionPoints GetNextLevelCost(const UpgradeKey& key, UpgradeLevel currentLevel) const;

		ProgressionPoints GetRebirthReward(Distance distance) const;
		ProgressionPoints GetPrestigeReward(Distance distance) const;
		ProgressionPoints GetAscendReward(Distance distance) const;

		WalkerProgression& GetRebirth() { return m_Rebirth; }
		const WalkerProgression& GetRebirth() const { return m_Rebirth; }

		WalkerProgression& GetPrestige() { return m_Prestige; }
		const WalkerProgression& GetPrestige() const { return m_Prestige; }
		void PrestigeReset();

		WalkerProgression& GetAscend() { return m_Ascend; }
		const WalkerProgression& GetAscend() const { return m_Ascend; }
		void AscendReset();

		RateBonus GetBonus(UpgradeKey key) const;

	private:
		WorkRate m_BaseCargoRate{10'000};
		WorkRate m_BaseJobRate{10};
		WorkRate m_BaseDiscoveryRate{1};

		Money m_BaseHireCost{100};		
		f32 m_HireCostBase{2.f};
		BaseTime m_MaxOfflineTime{OneHour};
		f32 m_OfflineTimeEfficiency{ 0.5f };
		Money m_CargoValue{ 1 };

		const TechManager& m_Tech;
		const MilestoneManager& m_Milestones;
		WalkerProgression m_Rebirth{};
		WalkerProgression m_Prestige{};
		WalkerProgression m_Ascend{};

	};
}