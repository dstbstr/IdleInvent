#pragma once

#include "InventLib/Technology/Invention.h"

#include <Mechanics/Unlockable.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {
	// Research then build each level



	// 100 technologies: 3 states (unlockable, purchasable, none)
    // 300 bits = 38 bytes (5 longs)
    // 18 technologies + 5 = 23 * 3 = 69 bits + 3 bits (which of the 5 techs) = 72 bits = 9 bytes (1 long, 1 byte)
	// 18 technologies * 3 levels = 36 bits = 4.5 bytes (1 long)

	// 5 storage techs * 7 levels
	// 8 levels (3 bits) * 7 storage inventions = 21 bits = 3 bytes
    /*
	namespace Technologies {
		struct SaveState {
            u8 StoneAgeLevel : 2 {0};
            u8 Specialty : 3 {0}; // if specialty is unset, then stone age is all 0, otherwise StoneAgeLevel corresponds to the specialty
            u8 SpecialtyStorageLevel : 3 {0};
            u8 OtherStorageLevels : 4 {0}; // level 0 or 1 for other 4 resources (in order, skip specialty)
            u8 ResourceLevels1and2 : 4 {0}; // tech 1 and 2
            u32 ResourcesLevels3to18{0}; // tech 3 - 18
		};

		void Initialize();
		void Load(const SaveState& saveState);
        void Save(SaveState& saveState);

		extern std::vector<Invention> StoneAgeTechs;
		extern std::vector<Invention> LaborTechs;
		extern std::vector<Invention> KnowledgeTechs;
		extern std::vector<Invention> WealthTechs;
		extern std::vector<Invention> InfluenceTechs;
		extern std::vector<Invention> MagicTechs;

		extern std::vector<Invention> StorageTechs;
	}
	*/
}