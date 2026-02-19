#pragma once
#include "GhostHunter/Teams/TeamMember.h"

#include <vector>

namespace GhostHunter {
	struct Team {
        std::unordered_map<MemberName, TeamMember> Members{};
		// fatigue, transport, credibility?
	};
}