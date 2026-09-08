#pragma once

#include "Pets/Pets/Pets.h"

namespace Pets::Leveling {
	struct Result {
        u32 PreviousLevel{};
        u32 CurrentLevel{};

		constexpr bool LeveledUp() const { return CurrentLevel > PreviousLevel; }
	};

	struct Event {
        PetKind Kind{};
        u32 PreviousLevel{};
        u32 CurrentLevel{};
	};

	[[nodiscard]] u32 NextLevelXp(u32 currentLevel);
    Result GrantXp(OwnedPet& pet, u32 xp);

	u32 XpForDepth(u32 xp, u32 depth);
}