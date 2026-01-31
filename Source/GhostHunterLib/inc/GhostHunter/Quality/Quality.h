#pragma once
#include "pch.h"

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
		Legendary,

		COUNT
	};

	std::string ToString(QualityType quality);
} // namespace GhostHunter