#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Rebirth/WalkerProgression.h"
#include "Walker/Travel/Vehicle.h"

#include <optional>

namespace Walker {
	class WalkerRates {
	public:
		WalkerRates(const TechManager& tech) : m_Tech(tech) {}

		WorkRate GetCargoWorkRate() const;
		WorkRate GetJobWorkRate() const;
		Acceleration GetBaseAccel(VehicleKind kind) const;
		Acceleration GetPoweredAccel(VehicleKind kind) const;
		Speed GetVehicleMaxSpeed(VehicleKind kind) const;
		Mass GetVehicleCapacity(VehicleKind kind) const;
		Money GetHireCost(u64 hiredCount, u64 count = 1) const;
		u64 GetMaxHireCount(Money funds, u64 hiredCount) const;
		std::optional<BaseTime> GetPreparationDuration() const;

		WalkerProgression& GetRebirth() { return m_Rebirth; }
		const WalkerProgression& GetRebirth() const { return m_Rebirth; }

		WalkerProgression& GetPrestiege() { return m_Prestiege; }
		const WalkerProgression& GetPrestiege() const { return m_Prestiege; }

		WalkerProgression& GetAscend() { return m_Ascend; }
		const WalkerProgression& GetAscend() const { return m_Ascend; }

		void Prestiege();
		void Ascend();

		std::pair<Quantity, double> GetBonus(u64 WalkerProgression::* points) const;

	private:
		WorkRate m_BaseCargoRate{10'000};
		WorkRate m_BaseJobRate{10};
		Money m_BaseHireCost{100};
		f32 m_HireCostBase{2.f};

		const TechManager& m_Tech;
		WalkerProgression m_Rebirth{};
		WalkerProgression m_Prestiege{};
		WalkerProgression m_Ascend{};

		Quantity Calculate(Quantity base, u64 WalkerProgression::* points) const;
	};
}