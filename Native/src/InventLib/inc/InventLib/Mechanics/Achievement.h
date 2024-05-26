#pragma once

#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/Mechanics/Effect.h"

#include <vector>
#include <string>

namespace Invent
{
	struct Achievement {
		std::string Name;
		std::string Description;

		std::vector<Effect> Effects;
	};
}