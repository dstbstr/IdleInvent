#pragma once
#include "Pets/Character/Party.h"
#include "Pets/Pets/Pets.h"

#include <Platform/NumTypes.h>

#include <vector>

namespace Pets {
	struct ResolvedPet {
		const PartyNode* Node{};
		size_t ParentIndex{};
		u32 Depth{};

		PetKind Kind{};
		u32 Attack{};
		u32 Piercing{};
		u32 PetCapacity{};
	};

	struct PartyResolution {
		std::vector<ResolvedPet> Pets{};
		f32 ActionRate{1.f};
	};

	namespace PartyResolver {
        [[nodiscard]] PartyResolution Resolve(const Party& party, const PetRoster& roster);
	}
}