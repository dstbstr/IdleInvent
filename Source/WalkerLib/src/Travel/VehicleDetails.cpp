#include "Walker/Travel/Vehicle.h"
#include "Walker/WalkerUnits.h"

#include <Instrumentation/Logging.h>

#include <array>

namespace {
	using namespace Walker;
	using namespace Walker::Literals;

	static constexpr std::array Details {
		VehicleDetails{}, // Unset
        //              Kind,       cost,               cap,     speed,   accel,    pAccel,  effic
        VehicleDetails{ VehicleKind::Foot,       Money::Pow10(0),    5_Kg,    1_mps,   1_mps2,   2_mps2,  0_spKg}, //Foot
        VehicleDetails{ VehicleKind::Bike,       Money::Pow10(4),    100_Kg,  15_mps,  1_mps2,   3_mps2,  0_spKg }, // Bike
        VehicleDetails{ VehicleKind::Car,        Money::Pow10(8),    1_t,     70_mps,  2_mps2,   6_mps2,  2_spKg }, // Car
        VehicleDetails{ VehicleKind::Boat,       Money::Pow10(12),   200_t,   25_mps,  1_mps2,   2_mps2,  10_spKg }, // Boat
        VehicleDetails{ VehicleKind::Plane,      Money::Pow10(16),   50_t,    250_mps, 1_mps2,   3_mps2,  3_spKg }, // Plane
        VehicleDetails{ VehicleKind::Jet,        Money::Pow10(20),   150_t,   600_mps, 2_mps2,   8_mps2,  2_spKg }, // Jet
        VehicleDetails{ VehicleKind::Rocket,     Money::Pow10(24),   3_Kt,    30_Kmps, 10_mps2,  30_mps2, 2_spKg }, // Rocket
        VehicleDetails{ VehicleKind::Starship,   Money::Pow10(28),   200_Mt,  1_Lyps,  10_Kmps2, 1_Mmps2, 4_hpKg}, // Starship
	};
}

namespace Walker {

    std::string ToString(VehicleKind kind) {
        switch(kind) {
            using enum VehicleKind;
		    case Unset: return "Unset";
            case Foot: return "Foot";
		    case Bike: return "Bike";
		    case Car: return "Car";
		    case Boat: return "Boat";
		    case Plane: return "Plane";
		    case Jet: return "Jet";
		    case Rocket: return "Rocket";
		    case Starship: return "Starship";
		    default: return "Unknown";
        }
    }

	const VehicleDetails& GetVehicleDetails(VehicleKind kind) {
        DR_ASSERT_MSG(kind > VehicleKind::Unset && kind < VehicleKind::COUNT, "Invalid vehicle kind");
        return Details.at(static_cast<size_t>(kind));
	}
}