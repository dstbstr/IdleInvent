#pragma once

#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"

#include "Resources/Resource.h"

namespace GhostHunter {
	class Life {
    public:
        Life();
        ~Life();

        void Update(BaseTime elapsed);

        const Market& GetMarket() const { return m_Market; }
        Inventory& GetInventory() { return m_Inventory; }
        const Inventory& GetInventory() const { return m_Inventory; }

    private:
        Inventory m_Inventory{};
        Market m_Market;
        size_t m_ToolPurchaseHandle{0};
	};
}