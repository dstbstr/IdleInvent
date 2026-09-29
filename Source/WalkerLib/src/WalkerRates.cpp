#include "Walker/WalkerRates.h"

namespace Walker {
	WorkRate WalkerRates::GetCargoWorkRate() const {
		return m_BaseCargoRate;
	}

	WorkRate WalkerRates::GetJobWorkRate() const {
		return m_BaseJobRate;
	}

	Speed WalkerRates::GetVehicleMaxSpeed(VehicleKind kind) const {
		return GetVehicleDetails(kind).MaxSpeed;
	}

	Mass WalkerRates::GetVehicleCapacity(VehicleKind kind) const {
		return GetVehicleDetails(kind).MaxCapacity;
	}
}