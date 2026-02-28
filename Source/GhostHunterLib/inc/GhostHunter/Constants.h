#pragma once
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Teams/TeamMember.h"
#include "GhostHunter/Tools/Tools.h"

namespace GhostHunter::Constants {
	namespace Unlocks {
        inline constexpr auto BestTool = ToolName::EmfDetector;
        inline constexpr auto BestLocation = LocationName::House;
        inline constexpr auto BestMember = MemberName::You;
        inline constexpr bool ShowRemainingResources = false;
        inline constexpr u64 StartingCash = 150;
        inline constexpr double DecayMultiplier = 0.9;
	}
}