#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"

#include "Manage/TickManager.h"
#include "Mechanics/Sale.h"
#include "Instrumentation/Logging.h"

namespace GhostHunter {
    static_assert(Tickable<Market>, "Expecting Market to be Tickable");

	Market::Market(ResourceCollection* resources, double decayRate) 
        : m_Resources(resources) 
        , m_DecayRate(decayRate) 
        , m_Accumulator {OneSecond, [this] { Accumulate(); } }
    {
        auto& services = ServiceLocator::Get();
        TickManager::Get().Register(m_Handles, *this);
        services.GetRequired<PubSub<Sale<Media>>>().Subscribe(m_Handles, [](const Sale<Media>& sale) {
            auto rc = SalesManager::TryGetValue<Media>(sale.Id, sale.Level);
            auto value = static_cast<u32>(rc.value_or(CreateRc<ResourceName>()).at(ResourceName::Cash).Current);
            Life::Get().GetMarket().m_Values.emplace_back(value);
        });
	}

	void Market::Tick(BaseTime elapsed) {
        m_Accumulator.Tick(elapsed);
	}

    void Market::Accumulate() {
        for(auto& value : m_Values) {
            m_Resources->at(ResourceName::Cash).Current += value;
            value = static_cast<u32>(value * m_DecayRate);
        }
        std::erase_if(m_Values, [](u32 value) {
            return value == 0; 
        });
    }
    void Market::Clear() {
        m_Values.clear();
    }

    void Market::Initialize() {
        Log::Debug("Market initialized"); 
        ServiceLocator::Get().CreateIfMissing<PubSub<Sale<Media>>>();
    }

    void Market::ShutDown() {
        Log::Debug("Market shut down"); 
    }
}