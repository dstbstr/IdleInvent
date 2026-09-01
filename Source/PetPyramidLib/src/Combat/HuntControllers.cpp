#include "Pets/Combat/HuntControllers.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEvents.h"

#include <utility>

namespace Pets {
    std::optional<ActionRequest> PartyAutoController::GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) {
        auto preyId = Social::ToFactionId(HuntFaction::Prey);
        auto target = encounter.Find([preyId](auto id, auto faction, const auto& combatant) {
            return faction == preyId && !encounter.IsDisabled(id);
        });
        if(!target.has_value()) return std::nullopt;

        // if eventually we support auto-capture, add the logic here
        ActionRequest result{
            .Kind = ActionRequestKind::Attack,
            .Target = *target,
        };
        return result;
    }

    std::optional<ActionRequest> PartyManualController::GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) {
        return std::exchange(CurrentAction, std::nullopt);
    }

    std::optional<ActionRequest> PreyController::GetNextAction(Combat::CombatantId actor, const HuntEncounter& encounter) {
        if(encounter.IsDisabled(actor)) return std::nullopt;

        auto instance = encounter.GetRoster().Get(actor);
        auto* stats = std::get_if<PreyStats>(&instance.Stats);

        if(!stats) return std::nullopt;

        auto action = ActionRequestKind::LookPretty;
        // simple stand-in logic
        if(stats->CurrentHp <= stats->MaxHp / 2) {
            if (stats->Dodge < 0.5f) {
                action = ActionRequestKind::Hide;
            } else {
                action = ActionRequestKind::Defend;
            }
        }

        return ActionRequest{
            .Kind = action,
            .Target = actor
        };
    }

}