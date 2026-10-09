#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Crew/WalkerTech.h"
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
	};

	class WalkerRates {
	public:
		WalkerRates(const TechManager& tech, const MilestoneManager& milestones) : m_Tech(tech), m_Milestones(milestones) {}

		WorkRate GetCargoWorkRate() const;
		WorkRate GetJobWorkRate() const;
		Acceleration GetBaseAccel(VehicleKind kind) const;
		Acceleration GetPoweredAccel(VehicleKind kind) const;
		Speed GetVehicleMaxSpeed(VehicleKind kind) const;
		Mass GetVehicleCapacity(VehicleKind kind) const;
		Money GetHireCost(u64 hiredCount, u64 count = 1) const;
		u64 GetMaxHireCount(Money funds, u64 hiredCount) const;
		std::optional<BaseTime> GetPreparationDuration() const;

		BaseTime GetOfflineTimeCapacity() const;
		f32 GetOfflineTimeEfficiency() const;

		u64 GetRebirthReward(Distance distance) const;
		WalkerProgression& GetRebirth() { return m_Rebirth; }
		const WalkerProgression& GetRebirth() const { return m_Rebirth; }

		u64 GetPrestiegeReward(Distance distance) const;
		WalkerProgression& GetPrestiege() { return m_Prestiege; }
		const WalkerProgression& GetPrestiege() const { return m_Prestiege; }
		void Prestiege();

		u64 GetAscendReward(Distance distance) const;
		WalkerProgression& GetAscend() { return m_Ascend; }
		const WalkerProgression& GetAscend() const { return m_Ascend; }
		void Ascend();

		RateBonus GetBonus(u64 WalkerProgression::* points) const;

	private:
		WorkRate m_BaseCargoRate{10'000};
		WorkRate m_BaseJobRate{10};
		Money m_BaseHireCost{100};
		f32 m_HireCostBase{2.f};
		BaseTime m_MaxOfflineTime{OneHour};
		f32 m_OfflineTimeEfficiency{ 0.5f };

		const TechManager& m_Tech;
		const MilestoneManager& m_Milestones;
		WalkerProgression m_Rebirth{};
		WalkerProgression m_Prestiege{};
		WalkerProgression m_Ascend{};

		Quantity Calculate(Quantity base, u64 WalkerProgression::* points) const;
	};
}