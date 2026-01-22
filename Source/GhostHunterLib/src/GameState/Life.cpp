#include "GhostHunter/GameState/Life.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/EventManager.h"

namespace GhostHunter {
	Life::Life() : m_Market({&m_Inventory.Resources}) {
        auto& services = ServiceLocator::Get();
        m_PurchaseToolHandle = 
            services.GetRequired<PubSub<Purchase<ToolName>>>().Subscribe([this](const Purchase<ToolName>& purchase) {
                m_Inventory.OwnedTools.push_back(Tool(purchase.Id, QualityType::Awful));
            });
		m_PurchaseLocationHandle =
		services.GetRequired<PubSub<Purchase<LocationName>>>().Subscribe([this](const Purchase<LocationName>& purchase) {
            auto& manager = ServiceLocator::Get().GetRequired<EventManager>();
            m_CurrentInvestigation = manager.StartEvent<Investigation>(purchase.Id);
			});
        m_InvestigationEndHandle = services.GetRequired<PubSub<EventEnd>>().Subscribe([this](const EventEnd& event) {
            if(event.Event->Handle == m_CurrentInvestigation) {
                m_CurrentInvestigation = InvalidEventHandle;
            }
        });
	}
	Life::~Life() {
		auto& services = ServiceLocator::Get();
		services.GetRequired<PubSub<Purchase<ToolName>>>().Unsubscribe(m_PurchaseToolHandle);
        services.GetRequired<PubSub<Purchase<LocationName>>>().Unsubscribe(m_PurchaseLocationHandle);
    }

	void Life::Update(BaseTime elapsed) {
		m_Market.Update(elapsed);
	}
} // namespace GhostHunter