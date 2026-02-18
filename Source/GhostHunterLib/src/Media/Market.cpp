#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Formatting.h"

#include "Manage/TickManager.h"
#include "Mechanics/Sale.h"
#include "Instrumentation/Logging.h"

namespace {
    constexpr double MinimumValue = 1.0;
    constexpr BaseTime PayoutInterval = OneSecond;

    struct PayoutBatch {
        u32 Cash{0};
        u32 Remaining{0};
    };

    u32 GetSalePrice(const GhostHunter::Media& media) {
        // TODO: Consider quality
        switch(media.Id) {
            using enum GhostHunter::MediaType;
            case Picture: return 10;
            case Interview: return 10;
            case Article: return 10;
            case Book: return 10;
            case Podcast: return 10;
            case TvShow: return 10;
            case Movie: return 10;
            default: break;
        }
        DR_ASSERT_MSG_LAZY(false, [type = media.Id]{
            return std::format("Unknown Media Type: {}", type);
        });
        return 10;
    }

    PayoutBatch FastForward(u32 currentValue, size_t ticks, double decayRate) {
        auto cash = 0ull;

        while(ticks-- && currentValue > MinimumValue) {
            cash += currentValue;
            currentValue = static_cast<u32>(currentValue * decayRate);
        }

        // clamp to avoid overflow
        cash = std::min<u64>(cash, std::numeric_limits<u32>::max());
        return {static_cast<u32>(cash), currentValue};        
    }
}
namespace GhostHunter {
	Market::Market(ResourceCollection* resources, double decayRate) 
        : m_Resources(resources) 
        , m_DecayRate(decayRate) {
        auto& services = ServiceLocator::Get();
        m_TickHandle = services.GetOrCreate<TickManager>().Register(*this);
        m_MediaHandle = services.GetRequired<PubSub<Sale<Media>>>().Subscribe([&](const Sale<Media>& media) {
            m_MarketMedia.emplace_back(MarketMedia{media.Item, GetSalePrice(media.Item)});
        });
	}

	Market::~Market() {
        auto& services = ServiceLocator::Get();
        if(auto* tickMgr = services.Get<TickManager>()) {
            tickMgr->Unregister(m_TickHandle);
        }
        if(auto* media = services.Get<PubSub<Sale<Media>>>()) {
            media->Unsubscribe(m_MediaHandle);
        }
    }

	void Market::Tick(BaseTime elapsed) {
        m_PayoutAccumulator += elapsed;
        auto ticks = m_PayoutAccumulator / PayoutInterval;
        if(ticks == 0) return;

        m_PayoutAccumulator %= PayoutInterval;

        for(auto& media : m_MarketMedia) {
            auto batch = FastForward(media.CurrentValue, ticks, m_DecayRate);
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