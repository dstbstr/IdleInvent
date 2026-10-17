#pragma once

#include <Platform/NumTypes.h>
#include <string>

namespace Walker {
	enum struct CrewRole : u8 {
		Idle,
		Traveling, // TODO: split into navigator, pilot, other roles?
		Stationed,
		Scout,
		Scientist,
		Engineer,

		COUNT
	};

	std::string ToString(CrewRole role);
}