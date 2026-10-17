#include "Walker/Crew/CrewRole.h"

namespace Walker {
	std::string ToString(CrewRole role) {
		switch (role) {
			using enum CrewRole;
			case Idle: return "Idle";
			case Traveling: return "Traveling";
			case Scout: return "Scout";
			case Scientist: return "Scientist";
			case Engineer: return "Engineer";
		}
		return "Unknown";
	}
}