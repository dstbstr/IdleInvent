#include "Walker/Journey/Endpoints.h"
#include "Walker/WalkerUnits.h"

#include <array>

namespace {
	using namespace Walker;
	using namespace Walker::Literals;

    static constexpr std::array Details = {
		EndpointDetails{}, // Unset
        //              Min distance, Max distance, Min cargo, Max cargo, Work/Kg
        EndpointDetails{10_m,         100_m,        50_Kg,     500_Kg,    10_j},   // Neighborhood
        EndpointDetails{1_Km,         50_Km,        250_Kg,    7'500_Kg,  20_j},   // InTown
        EndpointDetails{50_Km,        1_Mm,         1_Mg,      100_Mg,    50_j},   // InState
        EndpointDetails{500_Km,       3_Mm,         2_Mg,      500_Mg,    70_j},   // NearState
        EndpointDetails{3_Mm,         10_Mm,        20_Mg,     1_Kt,      100_j},  // FarState
        EndpointDetails{10_Mm,        40_Mm,        50_Mg,     5_Kt,      1_Kj},   // Earth
        EndpointDetails{100_Mm,       10'000_Gm,    200_t,     2_Mt,      5_Kj},   // SolarSystem
        EndpointDetails{1_Ly,         100_KLy,      1_Kt,      50_Mt,     100_Kj}, // MilkyWay
        EndpointDetails{100_KLy,      10_MLy,       2_Mt,      100_Gt,    200_Kj}, // NearGalaxy
        EndpointDetails{10_MLy,       1_GLy,        50_Mt,     1'000_Gt,  500_Kj}, // FarGalaxy
        EndpointDetails{1_GLy,        50_GLy,       100_Gt,    10'000_Gt, 1_Mj},   // EdgeOfUniverse
        EndpointDetails{50_GLy,       5'000_GLy,    1'000_Gt,  100'000_Gt, 5_Mj}   // GreatBeyond
    };

	static_assert(Details.size() == static_cast<size_t>(EndpointKind::COUNT));
}
namespace Walker {
    const EndpointDetails& GetEndpointDetails(EndpointKind kind) {
        DR_ASSERT_MSG(kind < EndpointKind::COUNT && kind > EndpointKind::Unset, "Invalid endpoint");
        return Details.at(static_cast<size_t>(kind));
    }
}