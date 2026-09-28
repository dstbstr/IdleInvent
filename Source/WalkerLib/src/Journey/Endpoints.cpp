#include "Walker/Journey/Endpoints.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/IRandom.h>

#include <algorithm>
#include <cmath>

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
		, Id(NextId++) {
        // TODO: roll distance, cargo, name etc.
		auto details = GetEndpointDetails(kind);
		auto& rand = ServiceLocator::Get().GetRequired<IRandom>();
		Name = GenerateEndpointName(kind, rand);

		auto LogRoll = [&](const Quantity& min, const Quantity& max) -> Quantity {
            if(min == max) return min;

            auto ratio = Quantity::Ratio(max, min);
            auto mul = std::pow(ratio, static_cast<double>(rand.GetNextFloat()));
            return std::clamp(min * mul, min, max);
		};

		DistanceFromHome = LogRoll(details.MinDistance, details.MaxDistance);
		InitialCargo = LogRoll(details.MinCargo, details.MaxCargo);
        RemainingCargo = InitialCargo;
        DeliveredCargo = Zero;
		UnitCargoWork = details.UnitCargoWork;
    }
}