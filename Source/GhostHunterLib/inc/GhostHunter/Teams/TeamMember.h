#pragma once
#include "GhostHunter/Tools/Tools.h"
#include "GhostHunter/Locations/Room.h"

namespace GhostHunter {
	struct TeamMember {
        std::string Name;
        std::optional<ToolName> CurrentTool;
        Room* CurrentRoom;
		//specialties/strengths/weaknesses
	};
}