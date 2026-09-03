#pragma once
#include "Pets/Combat/HuntCombatant.h"
#include "Pets/Character/Party.h"

namespace Pets::PartyProvider {
	HuntCombatant GetParty(const Party& party, const PetRoster& roster);
}