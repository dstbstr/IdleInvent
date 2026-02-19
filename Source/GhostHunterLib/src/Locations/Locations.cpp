#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Locations/Room.h"
#include "GhostHunter/Locations/Ghost.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "Manage/TickManager.h"
#include "DesignPatterns/ServiceLocator.h"

#include "GhostHunter/GameState/Life.h"

namespace GhostHunter {
    Location::Location(LocationName name) 
        : m_CooldownAccumulator(_LocationDetails::GetCooldownTime(name), [name](){Life::Get().GetLocation(name).OnCooldown(); })
        , m_Rooms(_LocationDetails::RoomsByLocation(name))
        , Id(name)
    {
        auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<InvestigationStart>>().Subscribe(m_Handles, [name](const auto&) {
            Life::Get().GetLocation(name).StartInvestigation();
        });
        services.GetRequired<PubSub<InvestigationEnd>>().Subscribe(m_Handles, [name](const auto&) {
            Life::Get().GetLocation(name).EndInvestigation();
        });
    }

    void Location::StartInvestigation() {
        m_TickHandle.reset();
    }

    void Location::EndInvestigation() {
        if(!m_TickHandle) {
            m_TickHandle = TickManager::Get().Register(*this);
        }
    }

    void Location::Tick(BaseTime elapsed) {
        m_CooldownAccumulator.Tick(elapsed);
    }

    void Location::OnCooldown() {
        m_TickHandle.reset();
        for(auto& room : m_Rooms) {
            room.AvailableResources = room.MaxResources;
        }
    }
        
    std::string Describe(LocationName location) { 
        return _LocationDetails::Describe(location); 
    }

} // namespace GhostHunter