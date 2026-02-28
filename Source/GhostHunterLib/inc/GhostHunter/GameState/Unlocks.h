#pragma once
#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Teams/TeamMember.h"

namespace GhostHunter {
	struct Unlocks {
        ToolName BestTool{ToolName::Unset};
        LocationName BestLocation{LocationName::Unset};
        MemberName BestMember{MemberName::Unset};

        bool ShowRemainingResources{false};
        u64 StartingCash{0};
        double DecayMultiplier{0.95};
	};
}