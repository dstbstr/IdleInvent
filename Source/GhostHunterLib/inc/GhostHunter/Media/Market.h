#pragma once

#include "GameState/GameTime.h"
#include "GhostHunter/Media/Media.h"
#include "Resources/Resource.h"
#include "Utilities/Handle.h"

namespace GhostHunter {
    struct GhostHunterResources;

    class Market {
        struct MarketMedia {
            MediaType MediaType;
            QualityType Level;
            u32 CurrentValue{0};
        };

        std::vector<MarketMedia> m_MarketMedia{};
        std::vector<ScopedHandle> m_Handles{};
        ResourceCollection* m_Resources{nullptr};
        BaseTime m_PayoutAccumulator{0};
        double m_DecayRate{0.95};

    public:
        Market(ResourceCollection* resources, double decayRate);

        void Tick(BaseTime elapsed);
        void Clear();

        static void Initialize();
        static void ShutDown();
    };
}