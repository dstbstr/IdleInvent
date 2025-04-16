#pragma once

#include "Mechanics/Unlockable.h"

#include <string>

struct Achievement {
	std::string Name;
	std::string Description;

	Unlockable Unlock;

	constexpr bool operator==(const Achievement& other) const {
		return Name == other.Name;
	}
};