#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

namespace {
    constexpr double DecayMultiplier = 0.95; // maybe make configurable for upgrades
    constexpr double MinimumValue = 1.0;
    constexpr BaseTime PayoutInterval = OneSecond;

    BaseTime payoutAccumulator{BaseTime::zero()};
    size_t mediaHandle = 0;
    GhostHunter::GhostHunterResources* resources{nullptr};

	struct MarketMedia {
        GhostHunter::Media Media;
        u32 CurrentValue{0};
    };
    
	std::vector<MarketMedia> marketMedia{};

	void OnMediaSold(const GhostHunter::Media& media) {
        marketMedia.emplace_back(MarketMedia{media, media.Value});
    }

    struct PayoutBatch {
        u32 Cash{0};
        u32 Remaining{0};
    };

    PayoutBatch FastForward(u32 currentValue, size_t ticks) {
        auto pow = std::pow(DecayMultiplier, ticks);
        auto ideal = static_cast<double>(currentValue) * (1.0 - pow) / (1.0 - DecayMultiplier);
        auto cash = static_cast<u32>(ideal);

        while(ticks-- && currentValue > 0) {
            cash += currentValue;
            currentValue = static_cast<u32>(currentValue * DecayMultiplier);
        }

        return {cash, currentValue};        
    }
}
namespace GhostHunter::Market {
	void Init() {
        auto& services = ServiceLocator::Get();
        mediaHandle = services.GetRequired<PubSub<Media>>().Subscribe(OnMediaSold);

		resources = services.Get<GhostHunterResources>();
	}

	void ShutDown() {
		auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
		pubSub.Unsubscribe(mediaHandle);
    }

	void Update(BaseTime elapsed) {
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
}