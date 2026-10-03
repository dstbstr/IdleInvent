#include "Walker/Travel/Vehicle.h"

#include "Walker/WalkerRates.h"

namespace Walker {
	f32 OwnedVehicle::FillRatio(const WalkerRates& rates) const {
		return static_cast<f32>(Mass::Ratio(CargoMass + CrewMass + FuelMass, rates.GetVehicleCapacity(Kind)));
	}

	Mass OwnedVehicle::GetTotalCapacity(const WalkerRates& rates) const {
		return rates.GetVehicleCapacity(Kind);
	}

	Mass OwnedVehicle::GetUsedCapacity() const {
		return CargoMass + CrewMass + FuelMass;
	}

	Mass OwnedVehicle::GetAvailableCapacity(const WalkerRates& rates) const {
		return std::max(Zero, GetTotalCapacity(rates) - GetUsedCapacity());
	}

	bool OwnedVehicle::CanHoldMoreCrew(const WalkerRates& rates) const {
		using namespace Walker::Literals;
		return GetAvailableCapacity(rates) >= 100_Kg;
	}

	Quantity OwnedVehicle::GetRemainingCrewCapacity(const WalkerRates& rates) const {
		using namespace Walker::Literals;
		return std::max(Zero, GetAvailableCapacity(rates) / 100_Kg);
	}

	void OwnedVehicle::SetCrew(u64 crewCount) {
		using namespace Walker::Literals;
		CrewMass = (crewCount - 1) * 100_Kg;
	}
}