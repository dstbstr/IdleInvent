#include "Pets/Pets/Leveling.h"

#include <Mechanics/ScaleFunc.h>

namespace Pets::Leveling {
    // TODO: Leveling cap, probably around 10 (theoretically we can reach 27, but could run into overflow problems)
	u32 NextLevelXp(u32 currentLevel) {
        DR_ASSERT_MSG(currentLevel > 0, "Current level must be greater than 0");
        if(currentLevel == 0) return 0;

        return 100 * Scale::Logarithmic<u32, 2>(currentLevel - 1);
	}

	Result GrantXp(OwnedPet& pet, u32 xp) {
        Result result{.PreviousLevel = pet.Level, .CurrentLevel = pet.Level};
        pet.Experience += xp;

        while(pet.Experience >= NextLevelXp(pet.Level)) {
            pet.Experience -= NextLevelXp(pet.Level);
            ++pet.Level;
        }
        result.CurrentLevel = pet.Level;
        return result;
    }

    u32 XpForDepth(u32 xp, u32 depth) {
        // each level gets half of the previous level
        return xp >> depth;
    }
}