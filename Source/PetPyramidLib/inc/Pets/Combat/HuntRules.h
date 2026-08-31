#pragma once

#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEvents.h"

#include <Combat/CombatTypes.h>
#include <Combat/CombatRoster.h>
#include <Combat/Encounter.h>

namespace Pets {
    struct HuntRules {
        bool IsDisabled(
            const Combat::Roster<HuntCombatant>& roster, 
            Combat::CombatantId actor) const;

        bool CanSubmit(
            const Combat::Roster<HuntCombatant>& roster, 
            Combat::CombatantId actor, 
            const ActionRequest& action ) const;

        Combat::ActionResolution<ActionResult> Update(
            Combat::Roster<HuntCombatant>& roster, 
            BaseTime elapsed) const;

        Combat::ActionResolution<ActionResult> EndTurn(
            Combat::Roster<HuntCombatant>& roster,
            Combat::CombatantId actor,
            Combat::TurnEnd endKind,
            const Combat::EncounterProgress& progress ) const;

        Combat::ActionResolution<ActionResult> Resolve(
            Combat::Roster<HuntCombatant>& roster, 
            Combat::CombatantId actor, 
            const ActionRequest& action) const;

    };
}