#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"

namespace {
	BaseTime TimeByLocation(GhostHunter::LocationName location) {
		switch(location) {
			using enum GhostHunter::LocationName;
			case Shed: return OneSecond * 30;
			case School: return OneSecond * 30;
			case House: return OneSecond * 30;
			case Church: return OneSecond * 30;
			case Hotel: return OneSecond * 30;
			case Prison: return OneSecond * 30;
			case Hospital: return OneSecond * 30;
			case Sanitorium: return OneSecond * 30;
			default: break;
		}
		DR_ASSERT_MSG(false, "Unknown Location: " + GhostHunter::ToString(location));
		return OneSecond * 30;
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

	std::string Investigation::Describe() const { 
		return std::format("Investigating {}", m_Location);
	}

	void Investigation::Initialize() {
		auto& services = ServiceLocator::Get();
        services.CreateIfMissing<PubSub<InvestigationStart>>();
        services.CreateIfMissing<PubSub<InvestigationEnd>>();
	}
}
