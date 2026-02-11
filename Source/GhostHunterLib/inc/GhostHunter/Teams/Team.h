#pragma once
#include "GhostHunter/Teams/TeamMember.h"

#include <vector>

namespace GhostHunter {
	struct Team {
        std::vector<TeamMember> Members;
		// fatigue, transport, credibility?
	};
}