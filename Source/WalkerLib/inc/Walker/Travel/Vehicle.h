#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

namespace Walker {
    class WalkerRates;

    enum struct VehicleKind : u8 { Unset, Foot, Bike, Car, Boat, Plane, Jet, Rocket, Starship, COUNT };

	std::string ToString(VehicleKind kind);

    struct VehicleChanged {};

	struct VehicleDetails {
        VehicleKind Kind{};
        Money Cost{};
        Mass MaxCapacity;
        Speed MaxSpeed;
        Acceleration BaseAcceleration;
        Acceleration PoweredAcceleration;
        FuelEfficiency FuelEfficiency;
	};
    
    const VehicleDetails& GetVehicleDetails(VehicleKind kind);

	struct OwnedVehicle {
        explicit OwnedVehicle(VehicleKind kind) : Kind(kind) {
            const auto& details = GetVehicleDetails(kind);
            TotalCapacity = details.MaxCapacity;
            MaxSpeed = details.MaxSpeed;
            BaseAcceleration = details.BaseAcceleration;
            PoweredAcceleration = details.PoweredAcceleration;
            Efficiency = details.FuelEfficiency;
        }

        VehicleKind Kind{};
        Mass TotalCapacity{}; // shared between cargo, crew, and fuel
        Mass CargoMass{}; 
        Mass CrewMass{};
        Mass FuelMass{};

        Speed MaxSpeed{};
        Acceleration BaseAcceleration{};
        Acceleration PoweredAcceleration{};

        FuelEfficiency Efficiency{};

        f32 FillRatio(const WalkerRates& rates) const;
        Mass GetTotalCapacity(const WalkerRates& rates) const;
		Mass GetAvailableCapacity(const WalkerRates& rates) const;
        Mass GetUsedCapacity() const;
        bool CanHoldMoreCrew(const WalkerRates& rates) const;
		Quantity GetRemainingCrewCapacity(const WalkerRates& rates) const;

        void SetCrew(u64 crewCount);
	};
}