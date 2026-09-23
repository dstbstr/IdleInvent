#pragma once

#include "Walker/WalkerUnits.h"

#include <Platform/NumTypes.h>

#include <string>

namespace Walker {
	enum struct EndpointKind : u8 { 
		Unset,
		Neighborhood, 
		InTown, 
		InState, 
		NearState, 
		FarState, 
		Earth, 
		SolarSystem,
        MilkyWay,
        NearGalaxy,
        FarGalaxy,
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

	struct EndpointInstance {
		explicit EndpointInstance(EndpointKind kind);

		EndpointKind Kind{};
		u64 Id{};
		std::string Name{};
		Distance DistanceFromHome{};
		Mass InitialCargo{};
		Mass RemainingCargo{};
		Mass DeliveredCargo{};
		Work UnitCargoWork{};
	private:
		inline static u64 NextId{1};
	};

	const EndpointDetails& GetEndpointDetails(EndpointKind kind);
}