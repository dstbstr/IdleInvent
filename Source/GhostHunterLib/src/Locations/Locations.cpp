#include "GhostHunter/Locations/Locations.h"
#include "Instrumentation/Logging.h"

namespace GhostHunter {
	std::vector<LocationName> GetAllLocationNames() {
		using enum LocationName;
		return {Shed, School, House, Church, Hotel, Prison, Hospital, Sanitorium};
    }

} // namespace GhostHunter