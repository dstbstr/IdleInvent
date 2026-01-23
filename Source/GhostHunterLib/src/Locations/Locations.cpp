#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "Mechanics/Purchasable.h"
#include "Instrumentation/Logging.h"

namespace GhostHunter {
	std::string Describe(LocationName location) {
		switch(location) {
			using enum LocationName;
        case Shed: return "Your mom's gardening shed. Haunted?";
        case School: return "Your younger brother's middle school. Spooky!";
        case House: return "The house that belonged to your neighbor before he died. Maybe he's still around";
        case Church: return "An abandoned church with boards on the windows. Now we're getting somewhere.";
        case Hotel: return "Opulence and ambience are nowhere to be seen anymore. This place is falling apart!";
        case Prison: return "Once home to the worst criminals, now home to restless spirits.";
        case Hospital: return "A decrepit hospital in eastern Europe. This place gives me the creeps during the day.";
        case Sanitorium: return "Who are we kidding, this place is certainly haunted, and wants you to reside here next.";
        default: break;
		}
        DR_ASSERT_MSG(false, "Unknown LocationName");
        return "....";
	}
} // namespace GhostHunter