#include "Walker/Journey/Endpoints.h"

namespace Walker {
    std::string ToString(EndpointKind kind) {
        switch(kind) {
            using enum EndpointKind;
            case Unset: return "Unset";
		    case Neighborhood: return "Neighborhood";
		    case InTown: return "In Town";
		    case InState: return "In State";
		    case NearState: return "Near State";
		    case FarState: return "Far State";
		    case Earth: return "Earth";
		    case SolarSystem: return "Moon";
            case MilkyWay: return "Milky Way";
            case NearGalaxy: return "Near Galaxy";
            case FarGalaxy: return "Far Galaxy";
            case EdgeOfUniverse: return "Edge Of The Universe";
            case GreatBeyond: return "The Great Beyond";
        }
        return "Unknown";
    }

    EndpointInstance::EndpointInstance(EndpointKind kind)
        : Kind(kind)
		, Id(NextId++)
        , Name(ToString(kind)) {
        // TODO: roll distance, cargo, name etc.
		auto details = GetEndpointDetails(kind);
        DistanceFromHome = details.DistanceFromHome;
		InitialCargo = details.InitialCargo;
        RemainingCargo = InitialCargo;
        DeliveredCargo = Zero;
		UnitCargoWork = details.UnitCargoWork;
    }
}