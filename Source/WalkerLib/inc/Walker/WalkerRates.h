#pragma once

#include "WalkerUnits.h"
#include "Rebirth/Rebirth.h"
#include "Rebirth/Prestiege.h"
#include "Rebirth/Ascend.h"
#include "Travel/Vehicle.h"

namespace Walker {
	class WalkerRates {
	public:
		WorkRate GetCargoWorkRate() const;
		WorkRate GetJobWorkRate() const;
		Speed GetVehicleMaxSpeed(VehicleKind kind) const;
		Mass GetVehicleCapacity(VehicleKind kind) const;
		Money GetHireCost(u64 hiredCount, u64 count = 1) const;

		WalkerRebirth& GetRebirth() { return m_Rebirth; }
		const WalkerRebirth& GetRebirth() const { return m_Rebirth; }

	private:
		WorkRate m_BaseCargoRate{10'000};
		WorkRate m_BaseJobRate{10};
		Money m_BaseHireCost{100};
		f32 m_HireCostBase{2.f};

		WalkerRebirth m_Rebirth{};
	};
}