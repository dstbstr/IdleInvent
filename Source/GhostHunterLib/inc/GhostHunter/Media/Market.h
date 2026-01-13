#pragma once

#include "GameState/GameTime.h"
#include "GhostHunter/Media/Media.h"
#include "Resources/Resource.h"

namespace GhostHunter {
    struct GhostHunterResources;

    class Market {
        struct MarketMedia {
            GhostHunter::Media Media;
            u32 CurrentValue{0};
        };

        std::vector<MarketMedia> marketMedia{};
        size_t mediaHandle = 0;
        //GhostHunterResources* resources{nullptr};
        ResourceCollection* resources{nullptr};
        BaseTime payoutAccumulator{BaseTime::zero()};

    public:
        Market();
        ~Market();

        void Update(BaseTime elapsed);
        void Clear();    
    };
}