#pragma once

#include "Pets/Combat/HuntTypes.h"

#include <Combat/CombatController.h>

#include <optional>

namespace Pets {
    struct PartyAutoController : Combat::ICombatController<ActionRequest, HuntEncounter> {
        std::optional<ActionRequest> GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) override;
    };

    struct PartyManualController : Combat::ICombatController<ActionRequest, HuntEncounter> {
        std::optional<ActionRequest> GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) override;
        std::optional<ActionRequest> CurrentAction;
    };

    struct PreyController : Combat::ICombatController<ActionRequest, HuntEncounter> {
        std::optional<ActionRequest> GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) override;
    };
}