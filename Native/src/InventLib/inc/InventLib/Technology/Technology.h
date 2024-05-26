#pragma once

#include "InventLib/Technology/Invention.h"
#include "InventLib/Mechanics/Unlockable.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {

	namespace Technologies {

		void Initialize();
		extern std::vector<Unlockable> StoneAgeTechs;
		extern std::vector<Unlockable> LaborTechs;
		extern std::vector<Unlockable> KnowledgeTechs;
		extern std::vector<Unlockable> WealthTechs;
		extern std::vector<Unlockable> InfluenceTechs;
		extern std::vector<Unlockable> MagicTechs;
	}
}