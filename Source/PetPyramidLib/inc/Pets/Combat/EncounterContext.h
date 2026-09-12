#pragma once

#include <vector>

namespace Pets {
    struct PartyResolution;
    struct PetRoster;
    struct Inventory;
    struct FieldEffect;

	struct EncounterContext {
        const PartyResolution& Party;
        const PetRoster& Roster;
        const Inventory& Inventory;
        const std::vector<FieldEffect>& FieldEffects;
	};
}