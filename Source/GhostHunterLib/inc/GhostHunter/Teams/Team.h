#pragma once
#include "GhostHunter/Teams/TeamMember.h"

#include <vector>

namespace GhostHunter {
	struct Team {
        std::vector<TeamMember> Members{};
		// fatigue, transport, credibility?

		Team() { 
			Members.reserve(static_cast<size_t>(MemberName::COUNT));
		}
	};
}