#pragma once

#include "GameState/GameTime.h"
#include "GhostHunter/Media/Media.h"
#include "Resources/Resource.h"
#include "Utilities/Handle.h"
#include "Mechanics/Accumulator.h"

namespace GhostHunter {
    struct GhostHunterResources;

    class Market {
        std::vector<u32> m_Values{};
        std::vector<ScopedHandle> m_Handles{};
        ResourceCollection* m_Resources{nullptr};
        double m_DecayRate{0.95};
        Accumulator m_Accumulator;

        void Accumulate();
    public:
        Market(ResourceCollection* resources, double decayRate);

        void Tick(BaseTime elapsed);
        void Clear();

        static void Initialize();
        static void ShutDown();
    };
}