#pragma once

#include "Pets/Combat/HuntRules.h"
#include "Pets/Combat/HuntEvents.h"
#include "Pets/Combat/HuntTypes.h"

#include <Combat/Encounter.h>

namespace Pets {
    using HuntEncounter = Combat::Encounter<HuntCombatant, ActionRequest, ActionResult, HuntRules>;
}