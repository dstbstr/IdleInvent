#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

namespace Walker {
    enum struct VehicleKind : u8 { Unset, Foot, Bike, Car, Boat, Plane, Jet, Rocket, Starship, COUNT };
    struct VehicleChanged {};

	struct VehicleDetails {
        CargoAmount InitialCapacity;
        CargoAmount MaxCapacity;
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
            MaxAcceleration = details.InitialAcceleration;
            Efficiency = details.InitialFuelEfficiency;
        }

        VehicleKind Kind{};
        CargoAmount TotalCapacity{}; // shared between cargo, crew, and fuel
        CargoAmount CargoMass{}; 
        CargoAmount CrewMass{};
        CargoAmount FuelMass{};

        Speed MaxSpeed{};
        Acceleration MaxAcceleration{};

        FuelEfficiency Efficiency{};

        f32 FillRatio() const {
            return static_cast<f32>(CargoAmount::Ratio(CargoMass + CrewMass + FuelMass, TotalCapacity));
        }

	};

}