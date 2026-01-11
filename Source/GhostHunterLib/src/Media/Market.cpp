#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

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

        while(ticks-- && currentValue > 0) {
            cash += currentValue;
            currentValue = static_cast<u32>(currentValue * DecayMultiplier);
        }

        // clamp to avoid overflow
        cash = std::min<u64>(cash, std::numeric_limits<u32>::max());
        return {static_cast<u32>(cash), currentValue};        
    }
}
namespace GhostHunter {
	Market::Market() {
        auto& services = ServiceLocator::Get();
        mediaHandle = services.GetRequired<PubSub<Media>>().Subscribe([&](const Media& media) {
            marketMedia.emplace_back(MarketMedia{media, media.Value});
        });

		resources = services.Get<GhostHunterResources>();
	}

	Market::~Market() {
		auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
		pubSub.Unsubscribe(mediaHandle);
    }

	void Market::Update(BaseTime elapsed) {
        payoutAccumulator += elapsed;
        auto ticks = payoutAccumulator / PayoutInterval;
        if(ticks == 0) return;

        payoutAccumulator %= PayoutInterval;

        for(auto& media : marketMedia) {
            auto batch = FastForward(media.CurrentValue, ticks);
            resources->AddResource(ResourceName::Cash, batch.Cash);
            media.CurrentValue = batch.Remaining;
        }
        std::erase_if(marketMedia, [](const MarketMedia& media) {
            return media.CurrentValue == 0; 
        });
	}

    void Market::Clear() {
        marketMedia.clear();
    }
}