#include "Walker/Journey/Endpoints.h"
#include "Walker/WalkerUnits.h"

#include <array>

namespace {
	using namespace Walker;
	using namespace Walker::Literals;

    static constexpr std::array Details = {
		EndpointDetails{}, // Unset
        EndpointDetails{10_m, 2_Kg, 10_j}, // across the street
        EndpointDetails{5_Km, 100_Kg, 20_j}, // across town
        EndpointDetails{1'00_Km, 1_Mg, 50_j}, // the capitol
        EndpointDetails{5'00_Km, 2_Mg, 70_j}, // next state
		EndpointDetails{5400_Km, 20_Mg, 100_j}, // Hawaii
		EndpointDetails{9000_Km, 50_Mg, 1_Kj}, // Europe
        EndpointDetails{384_Mm, 200_t, 5_Kj}, // Moon
        EndpointDetails{2'25_Gm, 1_Kt, 10'0_Kj}, // Mars
        EndpointDetails{5'90_Gm, 2_Mt, 20'0_Kj}, // Pluto
        EndpointDetails{437_Ly / 100, 50_Mt, 50'0_Kj}, // Alpha Centauri
        EndpointDetails{2540_Ly, 10'0_Gt, 100'0_Kj}, // Andromeda Galaxy
        EndpointDetails{54_MLy, 1000_Gt, 500'0_Kj}, // Virgo Supercluster
        EndpointDetails{465_GLy, 100000_Gt, 1_Mj} // Edge of the Universe
    };
}
namespace Walker {
    const EndpointDetails& GetEndpointDetails(EndpointKind kind) {
        DR_ASSERT_MSG(kind < EndpointKind::COUNT && kind > EndpointKind::Unset, "Invalid endpoint");
        return Details.at(static_cast<size_t>(kind));
    }
}