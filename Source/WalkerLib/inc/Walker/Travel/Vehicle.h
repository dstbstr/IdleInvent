#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

namespace Walker {
    enum struct VehicleKind : u8 { Unset, Foot, Bike, Car, Boat, Plane, Jet, Rocket, Starship, COUNT };
    struct VehicleChanged {};

	struct VehicleDetails {
        Mass InitialCapacity;
        Mass MaxCapacity;
        Speed InitialSpeed;
        Speed MaxSpeed;
        Acceleration InitialAcceleration;
        Acceleration MaxAcceleration;
        FuelEfficiency InitialFuelEfficiency;
        FuelEfficiency MaxFuelEfficiency;
	};
    
    const VehicleDetails& GetVehicleDetails(VehicleKind kind);

	struct OwnedVehicle {
        explicit OwnedVehicle(VehicleKind kind) : Kind(kind) {
            const auto& details = GetVehicleDetails(kind);
            TotalCapacity = details.InitialCapacity;
            MaxSpeed = details.InitialSpeed;
            BaseAcceleration = details.InitialAcceleration / 4;
            MaxAcceleration = details.InitialAcceleration;
            Efficiency = details.InitialFuelEfficiency;
        }

        VehicleKind Kind{};
        Mass TotalCapacity{}; // shared between cargo, crew, and fuel
        Mass CargoMass{}; 
        Mass CrewMass{};
        Mass FuelMass{};

        Speed MaxSpeed{};
        Acceleration BaseAcceleration{};
        Acceleration MaxAcceleration{};

        FuelEfficiency Efficiency{};

        f32 FillRatio() const {
            return static_cast<f32>(Mass::Ratio(CargoMass + CrewMass + FuelMass, TotalCapacity));
        }

	};

}