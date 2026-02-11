#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/GameState/World.h"
#include "GhostHunter/Tools/Tools.h"
#include "Mechanics/Purchasable.h"
#include "Manage/EventManager.h"

namespace GhostHunter {
	Life::Life() : m_Market(&m_Inventory.Resources) {
        auto OnToolPurchase = [this](const Purchase<ToolName>& tool) {
            m_Inventory.OwnedTools.emplace(tool.Id, Tool(tool.Id, Enum::Begin<QualityType>()));
        };
        auto OnLocPurchase = [this](const Purchase<LocationName>& loc) {
            auto& manager = ServiceLocator::Get().GetRequired<EventManager>();
            auto Unregister = [this, id=loc.Id](const IEvent&) { 
                m_CurrentInvestigation = InvalidHandle;
                if(auto* world = ServiceLocator::Get().Get<World>()) {
                    (*world).Locations.at(id).EndInvestigation();
                }
                for(auto& [_, tool] : m_Inventory.OwnedTools) {
                    tool.Stop();
                }
            };
            m_CurrentInvestigation = manager.StartEvent<Investigation>(Unregister, loc.Id);
            auto& world = ServiceLocator::Get().GetRequired<World>();
            world.Locations.at(loc.Id).StartInvestigation();
        };

        auto OnMediaPurchase = [this](const Purchase<MediaType>& purchase) {
            m_Inventory.CreatedMedia.emplace_back(purchase.Id, Enum::Begin<QualityType>());
        };

        Purchasables::Listen<ToolName>(m_PsHandles, OnToolPurchase);
        Purchasables::Listen<LocationName>(m_PsHandles, OnLocPurchase);
        Purchasables::Listen<MediaType>(m_PsHandles, OnMediaPurchase);
	}
	Life::~Life() {
        PubSubs::Unregister(m_PsHandles);
    }

    const Investigation* Life::GetCurrentInvestigation() const {
        return ServiceLocator::Get().GetRequired<EventManager>().GetEvent<Investigation>(m_CurrentInvestigation);
    }

} // namespace GhostHunter