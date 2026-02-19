#pragma once

#include "GhostHunter/GameState/Unlocks.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Teams/Team.h"

#include "Utilities/Handle.h"

namespace GhostHunter {
	class Life {
    public:
        static Life& Get() {
            return ServiceLocator::Get().GetRequired<Life>(); 
        }
        Life(const Unlocks& unlocks);

        const Market& GetMarket() const { return m_Market; }
        Market& GetMarket() { return m_Market; }
        
        Inventory& GetInventory() { return m_Inventory; }
        const Inventory& GetInventory() const { return m_Inventory; }

        Team& GetTeam() { return m_Team; }
        const Team& GetTeam() const { return m_Team; }

        Location& GetLocation(LocationName location) { return m_Locations.at(location); }
        const Location& GetLocation(LocationName location) const { return m_Locations.at(location); }
        std::vector<LocationName> GetUnlockedLocations() const;

        std::vector<ToolName> GetUnlockedTools() const { return m_UnlockedTools; }

        const Investigation* GetCurrentInvestigation() const;

    private:
        Inventory m_Inventory{};
        Market m_Market;
        Team m_Team{};
        
        std::vector<ScopedHandle> m_PsHandles{};
        Handle m_CurrentInvestigation{};
        std::unordered_map<LocationName, Location> m_Locations{};
        std::vector<ToolName> m_UnlockedTools{};

        void OnInvestigationStart(LocationName loc);
        void OnInvestigationEnd();
	};
}