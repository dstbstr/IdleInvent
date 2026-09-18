#include "Walker/Travel/Vehicle.h"
#include "Walker/WalkerUnits.h"

#include <Instrumentation/Logging.h>

#include <array>

namespace {
	using namespace Walker;
	using namespace Walker::Literals;

	static constexpr std::array Details {
		VehicleDetails{}, // Unset
        VehicleDetails{5_Kg, 30_Kg, 1_mps, 3_mps, Acceleration(10), 2_mps2, 0_spKg, 0_spKg}, //Foot
        VehicleDetails{20_Kg, 100_Kg, 5_mps, 15_mps, 1_mps2, 3_mps2, 0_spKg, 0_spKg }, // Bike
        VehicleDetails{500_Kg, 1500_Kg, 30_mps, 70_mps, 1_mps2, 6_mps2, 1_spKg, 2_spKg }, // Car
        VehicleDetails{20_t, 200_t, 10_mps, 25_mps, 0_mps2, 1_mps2, 5_spKg, 10_spKg }, // Boat
        VehicleDetails{5_t, 50_t, 100_mps, 250_mps, 1_mps2, 3_mps2, 1_spKg, 3_spKg }, // Plane
        VehicleDetails{20_t, 150_t, 250_mps, 600_mps, 2_mps2, 8_mps2, 1_spKg, 2_spKg }, // Jet
        VehicleDetails{300_t, 3_Kt, 8_Kmps, 30_Kmps, 10_mps2, 30_mps2, 1_spKg, 2_spKg }, // Rocket
        VehicleDetails{10_Mt, 200_Mt, 10_Gmps, 1_Lyps, 10_Kmps2, 1_Mmps2, 1_hpKg, 4_hpKg}, // Starship
	};
}

namespace Walker {
	const VehicleDetails& GetVehicleDetails(VehicleKind kind) {
        DR_ASSERT_MSG(kind > VehicleKind::Unset && kind < VehicleKind::COUNT, "Invalid vehicle kind");
        return Details.at(static_cast<size_t>(kind));
	}
}