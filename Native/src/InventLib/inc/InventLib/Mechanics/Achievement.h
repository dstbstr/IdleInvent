#pragma once

#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/Mechanics/Unlockable.h"

#include <vector>
#include <string>

namespace Invent
{
	struct Achievement {
		std::string Name;
		std::string Description;

		Unlockable Unlock;

		constexpr bool operator==(const Achievement& other) const {
			return Name == other.Name;
		}
	};
}