#pragma once
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Investigation/Evidence.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Tools/Tools.h"

#include "GameState/GameTime.h"
#include "Manage/EventManager.h"

#include <vector>
#include <string>

namespace GhostHunter {
    class Investigation : public IEvent {
        LocationName m_Location{LocationName::Unset};
        std::vector<Evidence> m_CollectedEvidence{};

    public:
        Investigation(LocationName location);
        void OnUpdate() override;
        std::string Describe() const override;

        LocationName GetLocation() const { return m_Location; }
    };
}