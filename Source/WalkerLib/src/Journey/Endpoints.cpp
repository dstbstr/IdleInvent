#include "Walker/Journey/Endpoints.h"

namespace Walker {
    std::string ToString(EndpointKind kind) {
        switch(kind) {
            using enum EndpointKind;
            case Unset: return "Unset";
		    case AcrossTheStreet: return "Across The Street";
		    case AcrossTown: return "Across Town";
		    case TheCapitol: return "The Capitol";
		    case NextState: return "Next State";
		    case Hawaii: return "Hawaii";
		    case Europe: return "Europe";
		    case Moon: return "Moon";
		    case Mars: return "Mars";
		    case Pluto: return "Pluto";
            case AlphaCentauri: return "Alpha Centauri";
            case AndromedaGalaxy: return "Andromeda";
            case VirgoSupercluster: return "Virgo Supercluster";
            case EdgeOfUniverse: return "Edge Of The Universe";
            case GreatBeyond: return "The Great Beyond";
        }
        return "Unknown";
    }
}