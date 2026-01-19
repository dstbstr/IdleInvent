#pragma once
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Tools/Tools.h"

#include "GameState/GameTime.h"

#include <chrono>
#include <vector>

namespace GhostHunter {
    class Investigation {
        LocationName m_Location{LocationName::Unset};
        std::vector<Media> m_CollectedMedia{};
        BaseTime m_Duration{BaseTime(0)};
        BaseTime m_Elapsed{BaseTime(0)};

    public:
        Investigation(LocationName location);
        void Update(BaseTime elapsed);
        bool IsComplete() const;
    };
}