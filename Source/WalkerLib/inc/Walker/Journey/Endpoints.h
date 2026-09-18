#pragma once

#include "Walker/WalkerUnits.h"

#include <Platform/NumTypes.h>

namespace Walker {
	enum struct EndpointKind : u8 { 
		Unset,
		AcrossTheStreet, 
		AcrossTown, 
		TheCapitol, 
		NextState, 
		Hawaii, 
		Europe, 
		Moon, 
		Mars, 
		Pluto,
        AlphaCentauri,
        AndromedaGalaxy,
        VirgoSupercluster,
        EdgeOfUniverse,
        GreatBeyond,
		
		COUNT
	};

	struct EndpointDetails {
        Distance DistanceFromHome{};
        CargoAmount InitialCargo{};
        Work UnitCargoWork{};
	};

	const EndpointDetails& GetEndpointDetails(EndpointKind kind);
}