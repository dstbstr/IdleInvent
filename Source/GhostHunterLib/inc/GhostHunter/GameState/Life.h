#pragma once

#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Teams/Team.h"

#include "Utilities/Handle.h"

namespace GhostHunter {
	class Life {
    public:
        Life();
        ~Life();

        const Market& GetMarket() const { return m_Market; }
        
        Inventory& GetInventory() { return m_Inventory; }
        const Inventory& GetInventory() const { return m_Inventory; }

        Team& GetTeam() { return m_Team; }
        const Team& GetTeam() const { return m_Team; }

        const Investigation* GetCurrentInvestigation() const;

    private:
        Inventory m_Inventory{};
        Market m_Market;
        Team m_Team{};
        
        std::vector<Handle> m_PsHandles{};
        Handle m_CurrentInvestigation{InvalidHandle};
	};
}