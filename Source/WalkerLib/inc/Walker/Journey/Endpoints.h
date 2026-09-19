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

	std::string ToString(EndpointKind kind);

	struct EndpointDetails {
        Distance DistanceFromHome{};
        Mass InitialCargo{};
        Work UnitCargoWork{};
	};

	const EndpointDetails& GetEndpointDetails(EndpointKind kind);
}