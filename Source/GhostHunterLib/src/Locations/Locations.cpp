#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Locations/Room.h"
#include "GhostHunter/Locations/Ghost.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "Manage/TickManager.h"
#include "DesignPatterns/ServiceLocator.h"

namespace GhostHunter {
    Location::Location(LocationName name) 
        : Id(name)
        , m_Rooms(_LocationDetails::RoomsByLocation(name))
        , m_CooldownAccumulator(_LocationDetails::GetCooldownTime(name), [this]() { OnCooldown();
        })
    {}

    void Location::StartInvestigation() {
        if(m_TickHandle != InvalidHandle) {
            ServiceLocator::Get().GetRequired<TickManager>().Unregister(m_TickHandle);
            m_TickHandle = InvalidHandle;
        }
    }

    void Location::EndInvestigation() {
        if(m_TickHandle == InvalidHandle) {
            m_TickHandle = ServiceLocator::Get().GetRequired<TickManager>().Register(*this);
        }
    }

    void Location::Tick(BaseTime elapsed) {
        m_CooldownAccumulator.Tick(elapsed);
    }

    void Location::OnCooldown() {
        ServiceLocator::Get().GetRequired<TickManager>().Unregister(m_TickHandle);
        m_TickHandle = InvalidHandle;
        for(auto& room : m_Rooms) {
            room.AvailableResources = room.MaxResources;
        }
    }
        
    std::string Describe(LocationName location) { 
        return _LocationDetails::Describe(location); 
    }

} // namespace GhostHunter