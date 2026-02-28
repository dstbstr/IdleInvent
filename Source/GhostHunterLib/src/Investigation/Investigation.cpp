#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Formatting.h"

namespace {
    inline constexpr auto Short = OneSecond * 10;
    inline constexpr auto Long = OneSecond * 30;

	// TODO: Probably move this to Location
	BaseTime TimeByLocation(GhostHunter::LocationName location) {
		switch(location) {
			using enum GhostHunter::LocationName;
			case Shed: return Short;
			case School: return Long;
			case House: return Long;
			case Church: return Long;
			case Hotel: return Long;
			case Prison: return Long;
			case Hospital: return Long;
			case Sanitorium: return Long;
			default: break;
		}
		DR_ASSERT_MSG(false, "Unknown Location: " + GhostHunter::ToString(location));
		return Long;
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
