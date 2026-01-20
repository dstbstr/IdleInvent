#pragma once
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Tools/Tools.h"

#include "GameState/GameTime.h"
#include "Mechanics/EventManager.h"

#include <vector>

namespace GhostHunter {
    class Investigation : public IEvent {
        LocationName m_Location{LocationName::Unset};
        std::vector<Media> m_CollectedMedia{};

    public:
        Investigation(LocationName location);
        void OnUpdate() override;
    };
}