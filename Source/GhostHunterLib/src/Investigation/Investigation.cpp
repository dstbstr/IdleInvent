#include "GhostHunter/Investigation/Investigation.h"

namespace GhostHunter {
    Investigation::Investigation(LocationName location) : IEvent(OneMinute * 5), m_Location(location) {
	}

	void Investigation::OnUpdate() {
		// collect media based on location and tools
		// Perhaps tools are events as well that we subscribe to
	}
}
