#pragma once

#include "Pets/Combat/HuntCombatant.h"
#include "Pets/Combat/HuntEvents.h"
#include "Pets/Combat/HuntRules.h"

#include <Combat/Encounter.h>
#include <Combat/CombatRunner.h>

namespace Pets {
    using HuntEncounter = Combat::Encounter<HuntCombatant, ActionRequest, ActionResult, HuntRules>;
    using HuntCombatRunner = Combat::Runner<HuntCombatant, ActionRequest, ActionResult, HuntRules>;
}