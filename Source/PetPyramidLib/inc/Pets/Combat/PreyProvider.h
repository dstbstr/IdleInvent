#pragma once

#include "Pets/Combat/HuntCombatant.h"
#include "Pets/Combat/EncounterContext.h"

namespace Pets::PreyProvider {
	HuntCombatant GetPrey(const EncounterContext& context);
}