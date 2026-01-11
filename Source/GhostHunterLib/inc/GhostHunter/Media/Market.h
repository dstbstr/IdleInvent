#pragma once

#include "GameState/GameTime.h"
#include "GhostHunter/Media/Media.h"

namespace GhostHunter {
    struct GhostHunterResources;

    class Market {
        struct MarketMedia {
            GhostHunter::Media Media;
            u32 CurrentValue{0};
        };

        std::vector<MarketMedia> marketMedia{};
        size_t mediaHandle = 0;
        GhostHunterResources* resources{nullptr};
        BaseTime payoutAccumulator{BaseTime::zero()};

        void OnMediaSold(const Media& media);

    public:
        Market();
        ~Market();

        void Update(BaseTime elapsed);
        void Clear();    
    };
}