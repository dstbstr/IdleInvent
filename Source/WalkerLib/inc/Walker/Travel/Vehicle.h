#pragma once

#include "Walker/WalkerUnits.h"
#include <Platform/NumTypes.h>

namespace Walker {
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

        f32 FillRatio() const {
            return static_cast<f32>(Mass::Ratio(CargoMass + CrewMass + FuelMass, TotalCapacity));
        }

		constexpr Mass GetAvailableCapacity() const {
			return std::max(Zero, TotalCapacity - (CargoMass + CrewMass + FuelMass));
		}

        constexpr bool CanHoldMoreCrew() const {
            using namespace Walker::Literals;
            return GetAvailableCapacity() > 100_Kg;
        }

        void SetCrew(u64 crewCount) {
			using namespace Walker::Literals;
			CrewMass = (crewCount - 1) * 100_Kg;
        }
	};

}