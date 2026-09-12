#pragma once

namespace Pets {
    struct PartyResolution;
    struct PetRoster;
    struct Inventory;

	struct EncounterContext {
        const PartyResolution& Party;
        const PetRoster& Roster;
        const Inventory& Inventory;
	};
}