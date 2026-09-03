#pragma once

#include "Pets/Character/Ability.h"
#include "Pets/Pets/Pets.h"

#include <GameState/GameTime.h>

#include <vector>

namespace Pets {
	struct LevelAbility {
		u32 Level{1};
		Ability Value{};
	};

	struct PetDetails {
		PetKind Kind{};
		u32 Attack{};
		u32 Piercing{};
		u32 PetCapacity{1};
		std::vector<LevelAbility> Abilities{};
	};

	namespace Details {
        [[nodiscard]] const PetDetails& Get(PetKind kind);
    }
}