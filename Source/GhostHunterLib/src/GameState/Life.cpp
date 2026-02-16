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
            OnInvestigationStart(loc.Id);
        };

        auto OnMediaPurchase = [this](const Purchase<MediaType>& purchase) {
            m_Inventory.CreatedMedia.emplace_back(purchase.Id, Enum::Begin<QualityType>());
        };

        Purchasables::Listen<ToolName>(m_PsHandles, OnToolPurchase);
        Purchasables::Listen<LocationName>(m_PsHandles, OnLocPurchase);
        Purchasables::Listen<MediaType>(m_PsHandles, OnMediaPurchase);

        for(auto name = Enum::Begin<LocationName>(); name != Enum::End<LocationName>(); name = Enum::Increment(name)) {
            m_Locations.emplace(name, Location(name));
        }
	}
	Life::~Life() {
        PubSubs::Unregister(m_PsHandles);
    }

    const Investigation* Life::GetCurrentInvestigation() const {
        return ServiceLocator::Get().GetRequired<EventManager>().GetEvent<Investigation>(m_CurrentInvestigation);
    }

    void Life::OnInvestigationStart(LocationName loc) {
        auto& services = ServiceLocator::Get();
        auto& manager = services.GetRequired<EventManager>();

        auto Unregister = [this, &services](const IEvent&) { OnInvestigationEnd(); };

        m_CurrentInvestigation = manager.StartEvent<Investigation>(Unregister, loc);
        services.GetRequired<PubSub<InvestigationStart>>().Publish({});
    }

    void Life::OnInvestigationEnd() {
        m_CurrentInvestigation.Reset();
        ServiceLocator::Get().GetRequired<PubSub<InvestigationEnd>>().Publish({});
    }
} // namespace GhostHunter