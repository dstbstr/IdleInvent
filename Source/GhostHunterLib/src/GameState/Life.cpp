#include "GhostHunter/GameState/Life.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Mechanics/Purchasable.h"
#include "Manage/EventManager.h"

namespace GhostHunter {
	Life::Life() : m_Market({&m_Inventory.Resources}) {
        auto OnToolPurchase = [this](const Purchase<ToolName>& tool) {
            m_Inventory.OwnedTools.push_back(Tool(tool.Id, Enum::Begin<QualityType>()));
        };
        auto OnLocPurchase = [this](const Purchase<LocationName>& loc) {
            auto& manager = ServiceLocator::Get().GetRequired<EventManager>();
            auto Unregister = [this](const IEvent&) { m_CurrentInvestigation = InvalidHandle; };
            m_CurrentInvestigation = manager.StartEvent<Investigation>(Unregister, loc.Id);
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