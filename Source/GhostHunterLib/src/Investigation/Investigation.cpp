#include "GhostHunter/Investigation/Investigation.h"

namespace {
	BaseTime TimeByLocation(GhostHunter::LocationName location) {
		switch(location) {
			using enum GhostHunter::LocationName;
			case Shed: return OneMinute;
			case School: return OneMinute;
			case House: return OneMinute;
			case Church: return OneMinute;
			case Hotel: return OneMinute;
			case Prison: return OneMinute;
			case Hospital: return OneMinute;
			case Sanitorium: return OneMinute;
			default: break;
		}
		DR_ASSERT_MSG(false, "Unknown Location: " + GhostHunter::ToString(location));
		return OneMinute;
    }
}
namespace GhostHunter {
    Investigation::Investigation(LocationName location) 
		: IEvent(TimeByLocation(location))
		, m_Location(location) {
	}

	void Investigation::OnUpdate() {
		// collect media based on location and tools
		// Perhaps tools are events as well that we subscribe to
	}
}
