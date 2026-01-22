#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Mechanics/Sale.h"

namespace {
    constexpr double DecayMultiplier = 0.95; // maybe make configurable for upgrades
    constexpr double MinimumValue = 1.0;
    constexpr BaseTime PayoutInterval = OneSecond;

    struct PayoutBatch {
        u32 Cash{0};
        u32 Remaining{0};
    };

    PayoutBatch FastForward(u32 currentValue, size_t ticks) {
        auto cash = 0ull;

        while(ticks-- && currentValue > MinimumValue) {
            cash += currentValue;
            currentValue = static_cast<u32>(currentValue * DecayMultiplier);
        }

        // clamp to avoid overflow
        cash = std::min<u64>(cash, std::numeric_limits<u32>::max());
        return {static_cast<u32>(cash), currentValue};        
    }
}
namespace GhostHunter {
	Market::Market(ResourceCollection* resources) : m_Resources(resources) {
        auto& services = ServiceLocator::Get();
        m_MediaHandle = services.GetRequired<PubSub<Sale<Media>>>().Subscribe([&](const Sale<Media>& media) {
            m_MarketMedia.emplace_back(MarketMedia{media.Item, media.Item.Value});
        });
	}

	Market::~Market() {
		auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<Media>>>();
		pubSub.Unsubscribe(m_MediaHandle);
    }

	void Market::Update(BaseTime elapsed) {
        m_PayoutAccumulator += elapsed;
        auto ticks = m_PayoutAccumulator / PayoutInterval;
        if(ticks == 0) return;

        m_PayoutAccumulator %= PayoutInterval;

        for(auto& media : m_MarketMedia) {
            auto batch = FastForward(media.CurrentValue, ticks);
            m_Resources->at(ResourceName::Cash).Current += batch.Cash;
            media.CurrentValue = batch.Remaining;
        }
        std::erase_if(m_MarketMedia, [](const MarketMedia& media) {
            return media.CurrentValue == 0; 
        });
	}

    void Market::Clear() {
        m_MarketMedia.clear();
    }

    void Market::Initialize() {
        Log::Debug("Market initialized"); 
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<PubSub<Sale<Media>>>();
    }

    void Market::ShutDown() {
        Log::Debug("Market shut down"); 
    }
}