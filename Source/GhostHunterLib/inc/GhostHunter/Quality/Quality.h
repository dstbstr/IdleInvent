#pragma once
#include <Platform/NumTypes.h>

#include <string>

namespace GhostHunter {
	enum struct QualityType : u8 {
		Unset,

		Awful,
		Bad,
		Okay,
		PrettyGood,
		Great,
		Excellent,
		Wonderful,
		Legendary
	};

	std::string ToString(QualityType quality);
} // namespace GhostHunter