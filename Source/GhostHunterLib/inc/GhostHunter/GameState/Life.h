#pragma once

#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Media/Market.h"

#include "Mechanics/EventManager.h"

namespace GhostHunter {
	class Life {
    public:
        Life();
        ~Life();

        void Update(BaseTime elapsed);

        const Market& GetMarket() const { return m_Market; }
        
        Inventory& GetInventory() { return m_Inventory; }
        const Inventory& GetInventory() const { return m_Inventory; }

        const Investigation* GetCurrentInvestigation() const {
            return ServiceLocator::Get().GetRequired<EventManager>().GetEvent<Investigation>(m_CurrentInvestigation);
            
        }
    private:
        Inventory m_Inventory{};
        Market m_Market;
        
        std::vector<Handle> m_PsHandles{};

        Handle m_CurrentInvestigation{InvalidHandle};
	};
}