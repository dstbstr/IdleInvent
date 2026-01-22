#pragma once
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Tools/Tools.h"

#include "GameState/GameTime.h"
#include "Mechanics/EventManager.h"

#include <vector>

namespace GhostHunter {
    class Investigation : public IEvent {
        LocationName m_Location{LocationName::Unset};
        std::vector<Evidence> m_CollectedEvidence{};

    public:
        Investigation(LocationName location);
        void OnUpdate() override;

        LocationName GetLocation() const { return m_Location; }
    };
}