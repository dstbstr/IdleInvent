#pragma once

#include "GameState/GameTime.h"
#include "GhostHunter/Media/Media.h"
#include "Resources/Resource.h"
#include "Utilities/Handle.h"

namespace GhostHunter {
    struct GhostHunterResources;

    class Market {
        struct MarketMedia {
            GhostHunter::Media Media;
            u32 CurrentValue{0};
        };

        std::vector<MarketMedia> m_MarketMedia{};
        Handle m_MediaHandle = 0;
        ResourceCollection* m_Resources{nullptr};
        BaseTime m_PayoutAccumulator{BaseTime::zero()};

    public:
        Market(ResourceCollection* resources);
        ~Market();

        void Update(BaseTime elapsed);
        void Clear();

        static void Initialize();
        static void ShutDown();
    };
}