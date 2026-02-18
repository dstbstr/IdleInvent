#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/Tools/Tools.h"
#include "Mechanics/Purchasable.h"
#include "Manage/EventManager.h"

namespace GhostHunter {
    Life::Life(const Unlocks& unlocks) 
        : m_Market(&m_Inventory.Resources, unlocks.DecayMultiplier) {
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

        m_Locations.reserve(static_cast<size_t>(LocationName::COUNT));
        for(auto name = Enum::Begin<LocationName>(); name <= unlocks.BestLocation; name = Enum::IncrementUnbounded(name)) {
            m_Locations.emplace(name, Location(name));
        }
        m_Team.Members.reserve(static_cast<size_t>(MemberName::COUNT));
        for(auto name = Enum::Begin<MemberName>(); name <= unlocks.BestMember; name = Enum::IncrementUnbounded(name)) {
            m_Team.Members.emplace_back(name);
        }
        m_UnlockedTools.reserve(static_cast<size_t>(ToolName::COUNT));
        for(auto name = Enum::Begin<ToolName>(); name <= unlocks.BestTool; name = Enum::IncrementUnbounded(name)) {
            m_UnlockedTools.emplace_back(name);
        }

        m_Inventory.Resources = CreateRc<ResourceName>(std::pair{ResourceName::Cash, unlocks.StartingCash});
	}

    const Investigation* Life::GetCurrentInvestigation() const {
        return ServiceLocator::Get().GetRequired<EventManager>().GetEvent<Investigation>(m_CurrentInvestigation);
    }

    void Life::OnInvestigationStart(LocationName loc) {
        auto& services = ServiceLocator::Get();
        auto& manager = services.GetRequired<EventManager>();

        auto Unregister = [this](const IEvent&) { OnInvestigationEnd(); };

        m_CurrentInvestigation = manager.StartEvent<Investigation>(Unregister, loc);
        services.GetRequired<PubSub<InvestigationStart>>().Publish({});
    }

    void Life::OnInvestigationEnd() {
        m_CurrentInvestigation.Reset();
        ServiceLocator::Get().GetRequired<PubSub<InvestigationEnd>>().Publish({});
    }

    std::vector<LocationName> Life::GetUnlockedLocations() const {
        std::vector<LocationName> result;
        result.reserve(m_Locations.size());

        for(const auto& [name, loc] : m_Locations) {
            result.push_back(name);
        }
        return result;
    }
} // namespace GhostHunter