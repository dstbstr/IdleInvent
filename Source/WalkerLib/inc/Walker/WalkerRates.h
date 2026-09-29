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

	private:
		WorkRate m_BaseCargoRate{10'000};
		WorkRate m_BaseJobRate{10};

	};
}