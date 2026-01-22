#include "GhostHunter/GameState/Life.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/EventManager.h"

namespace GhostHunter {
	Life::Life() : m_Market({&m_Inventory.Resources}) {
        auto& services = ServiceLocator::Get();
        m_PsHandles.push_back(
            services.GetRequired<PubSub<Purchase<ToolName>>>().Subscribe([this](const Purchase<ToolName>& purchase) {
                m_Inventory.OwnedTools.push_back(Tool(purchase.Id, QualityType::Awful));
            }));
		m_PsHandles.push_back(
		    services.GetRequired<PubSub<Purchase<LocationName>>>().Subscribe([this](const Purchase<LocationName>& purchase) {
            auto& manager = ServiceLocator::Get().GetRequired<EventManager>();
            m_CurrentInvestigation = manager.StartEvent<Investigation>(purchase.Id);
			}));
        m_PsHandles.push_back(
            services.GetRequired<PubSub<EventEnd>>().Subscribe([this](const EventEnd& event) {
            if(event.Event->Handle == m_CurrentInvestigation) {
                m_CurrentInvestigation = InvalidHandle;
            }
            }));
	}
	Life::~Life() {
        PubSubs::Unregister(m_PsHandles);
    }

	void Life::Update(BaseTime elapsed) {
		m_Market.Update(elapsed);
	}
} // namespace GhostHunter