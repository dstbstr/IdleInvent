#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Locations/Room.h"
#include "GhostHunter/Locations/Ghost.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

namespace {
    ResourceCollection MakeRc(s64 images, s64 audio, s64 video, s64 physical) {
        using enum GhostHunter::ResourceName;
        return CreateRc<GhostHunter::ResourceName>(
            std::pair{Images, images}, 
            std::pair{Audio, audio}, 
            std::pair{Video, video}, 
            std::pair{Physical, physical});
    }

    std::vector<GhostHunter::Room> RoomsByLocation(GhostHunter::LocationName location) {
        std::vector<GhostHunter::Room> result{};
        switch(location) {
            using enum GhostHunter::LocationName;
            case Shed: { 
                result.emplace_back("Entry Way", MakeRc(5, 8, 2, 10), std::nullopt);
                break;
                }
            
        }
    }
}

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

    Location::Location(LocationName name) 
        : Id(name)
        , Rooms(RoomsByLocation(name)) 
    {}

    void StartCooldown() {
        if(m_TickHandle != InvalidHandle) return;
        m_TickHandle = ServiceLocator::Get().GetRequired<TickManager>().Register(*this);
    }

    void StartInvestigation() {
        if(!m_CooledDown) return;
    }

        
} // namespace GhostHunter