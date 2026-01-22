#include "GhostHunter/GameState/Life.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Mechanics/Purchasable.h"

namespace GhostHunter {
	Life::Life() : m_Market({&m_Inventory.Resources}) {
        auto& services = ServiceLocator::Get();
        m_ToolPurchaseHandle =
            services.GetRequired<PubSub<Purchase<ToolName>>>().Subscribe([this](const Purchase<ToolName>& purchase) {
                m_Inventory.OwnedTools.push_back(Tool(purchase.Id, QualityType::Awful));
            });
	}
	Life::~Life() {
		auto& services = ServiceLocator::Get();
		services.GetRequired<PubSub<Purchase<ToolName>>>().Unsubscribe(m_ToolPurchaseHandle);
    }

	void Life::Update(BaseTime elapsed) {
		m_Market.Update(elapsed);
	}
} // namespace GhostHunter