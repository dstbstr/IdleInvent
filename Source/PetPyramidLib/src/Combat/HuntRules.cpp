#include "Pets/Combat/HuntRules.h"
#include "Pets/Combat/HuntTypes.h"
#include "Pets/Combat/HuntEvents.h"
#include "Pets/Inventory/Items.h"

#include <Instrumentation/Logging.h>

#include <format>
#include <vector>

namespace {
    using namespace Combat;
    using namespace Pets;

    struct ResolutionContext {
        CombatantId ActorId;
        CombatantId TargetId;
        HuntCombatant& Actor;
        HuntCombatant& Target;
        ActionRequestContext Context;
    };

    ActionResult MakeEvent(ActionResultKind kind, const ResolutionContext& requestContext, ActionResultContext resultContext = std::monostate{}) {
        return {.Kind = kind, .Actor = requestContext.ActorId, .Target = requestContext.TargetId, .Context = resultContext};
    }

    ActionResolution<ActionResult> ResolveAttack(const ResolutionContext& context) {
        auto* attackerStats = std::get_if<PartyStats>(&context.Actor.Stats);
        auto* defenderStats = std::get_if<PreyStats>(&context.Target.Stats);
        DR_ASSERT_MSG(attackerStats, "Attacker must have PartyStats");
        DR_ASSERT_MSG(defenderStats, "Defender must have PreyStats");
        if(!attackerStats || !defenderStats) return {};

        auto amount = attackerStats->Attack;
        auto reduced = defenderStats->Armor - static_cast<s32>(attackerStats->Piercing);
        reduced = std::clamp(reduced, 0, static_cast<s32>(amount));
        amount -= reduced;

        defenderStats->CurrentHp -= amount;
        ActionResolution<ActionResult> result{};
        result.Events.push_back(MakeEvent(ActionResultKind::Damaged, context, static_cast<s32>(amount)));
        if(defenderStats->CurrentHp <= 0) {
            result.Events.push_back(MakeEvent(ActionResultKind::PreyKilled, context));
            result.EncounterFinished = true;
        }
        return result;
    }

    ActionResolution<ActionResult> ResolveCapture(const ResolutionContext& context) {
        auto* attackerStats = std::get_if<PartyStats>(&context.Actor.Stats);
        auto* defenderStats = std::get_if<PreyStats>(&context.Target.Stats);
        DR_ASSERT_MSG(attackerStats, "Attacker must have PartyStats");
        DR_ASSERT_MSG(defenderStats, "Defender must have PreyStats");
        if(!attackerStats || !defenderStats) return {};
    
        ActionResolution<ActionResult> result{};
        if(defenderStats->CurrentHp > static_cast<s32>(defenderStats->MaxHp * defenderStats->CaptureHpLevel)) {
            result.Events.push_back(MakeEvent(ActionResultKind::CaptureFailed, context));
            return result;
        }

        // TODO: calculate capture chance
        result.Events.push_back(MakeEvent(ActionResultKind::Captured, context));
        result.EncounterFinished = true;
        return result;
    }

    ActionResolution<ActionResult> ResolveFlee(const ResolutionContext& context) {
        ActionResolution<ActionResult> result{};
        result.Events.push_back(MakeEvent(ActionResultKind::PreyFled, context));
        result.EncounterFinished = true;
        return result;
    }

    ActionResolution<ActionResult> ResolveDefend(const ResolutionContext& context) {
        auto* preyStats = std::get_if<PreyStats>(&context.Actor.Stats);
        // if players can defend for some reason, add it here
        DR_ASSERT_MSG(preyStats, "Defender must have PreyStats");
        if(!preyStats) return {};

        auto amount = 10; // TODO: Calculate
        preyStats->Armor += amount;

        ActionResolution<ActionResult> result{};
        result.Events.push_back(MakeEvent(ActionResultKind::Defended, context, amount));
        return result;
    }

    ActionResolution<ActionResult> ResolveHide(const ResolutionContext& context) {
        auto* preyStats = std::get_if<PreyStats>(&context.Actor.Stats);
        // if players can hide for some reason, add it here
        DR_ASSERT_MSG(preyStats, "Defender must have PreyStats");
        if(!preyStats) return {};

        auto amount = 0.1f;
        preyStats->Dodge += amount;

        ActionResolution<ActionResult> result{};
        result.Events.push_back(MakeEvent(ActionResultKind::Hidden, context));
        return result;
    }

    ActionResolution<ActionResult> ResolveItem(const ResolutionContext& context, Inventory& inventory) {
        ActionResolution<ActionResult> result{};

        auto* item = std::get_if<ItemContext>(&context.Context);
        DR_ASSERT_MSG(item, "Item context must be present for item action");
        if(!item) return result;

        auto* preyStats = std::get_if<PreyStats>(&context.Target.Stats);
        auto* partyStats = std::get_if<PartyStats>(&context.Actor.Stats);

        if (!inventory.Contains(item->ItemId)) {
            DR_ASSERT_MSG(false, "Actor does not have item in inventory");
            return result;
        }

        switch (item->ItemId) {
            using enum CombatItemKind;
            case Distraction: {
                DR_ASSERT_MSG(preyStats, "Target must have PreyStats for Distraction item");
                if(!preyStats) return result;
                preyStats->FleeTime += OneMinute;
                break;
            }
            case Net: {
                DR_ASSERT_MSG(preyStats, "Target must have PreyStats for Net item");
                if(!preyStats) return result;
                preyStats->CaptureChance *= 1.2f;
                break;
            }
            case AtkPotion: {
                DR_ASSERT_MSG(partyStats, "Actor must have PartyStats for AtkPotion item");
                if(!partyStats) return result;
                partyStats->Attack += 10;
                break;
            }
            case SpdPotion: {
                DR_ASSERT_MSG(partyStats, "Actor must have PartyStats for SpdPotion item");
                if(!partyStats) return result;
                result.ScheduleChanges.push_back({ 
                    .Combatant = context.ActorId, 
                    .SpeedModifier = {.Mul = 1.5f}
                });
                break;
            }
            case PiercePotion: {
                DR_ASSERT_MSG(partyStats, "Actor must have PartyStats for PiercePotion item");
                if(!partyStats) return result;
                partyStats->Piercing += 10;
                break;
            }
            case Poison: {
                DR_ASSERT_MSG(preyStats, "Target must have PreyStats for Poison item");
                if(!preyStats) return result;
                // something to apply poison effect
                break;
            }
        }

        inventory.Consume(item->ItemId);

        result.Events.push_back(MakeEvent(ActionResultKind::ItemUsed, context, item->ItemId));
        return result;
    }
}

namespace Pets {
	using namespace Combat;

	bool HuntRules::IsDisabled(const Roster<HuntCombatant>& roster, CombatantId actor) const {
        if(!roster.Contains(actor)) return true;

        const auto& combatant = roster.Get(actor);
        if(auto* stats = std::get_if<PreyStats>(&combatant.Stats)) {
            return stats->CurrentHp <= 0;
        }
        return false;
    }

    bool HuntRules::CanSubmit(const Roster<HuntCombatant>& roster, CombatantId actor, const ActionRequest& action) const {
        if(!roster.Contains(actor) || !roster.Contains(action.Target)) return false;

        auto validTarget = true;
        if(auto* stats = std::get_if<PreyStats>(&roster.Get(action.Target).Stats)) {
            // can't target dead prey
            validTarget &= stats->CurrentHp > 0;
        }

        return !IsDisabled(roster, actor) && validTarget;
    }

    ActionResolution<ActionResult> HuntRules::Update(Roster<HuntCombatant>& roster, BaseTime elapsed) const {
        auto preyFled = false;
        roster.Visit([&, elapsed](CombatantId id, Social::FactionId, HuntCombatant& combatant) {
            if(auto* stats = std::get_if<PreyStats>(&combatant.Stats)) {
                if(IsDisabled(roster, id)) return;

                stats->FleeTime -= std::min(elapsed, stats->FleeTime);
                preyFled |= stats->FleeTime == BaseTime(0);
            }
        });

        if(preyFled) {
            return {.Events = {{.Kind = ActionResultKind::PreyFled}}, .EncounterFinished = true};
        }

        return {};
    }

    ActionResolution<ActionResult> HuntRules::EndTurn(
        Roster<HuntCombatant>& roster,
        CombatantId actor,
        TurnEnd endKind,
        const EncounterProgress& progress
    ) const {
        return {
            .Events = {
                {.Kind = endKind == TurnEnd::Skipped ? ActionResultKind::TurnSkipped : ActionResultKind::TurnEnded,
                 .Actor = actor}
            }
        };
    }

    ActionResolution<ActionResult> HuntRules::Resolve(
        Roster<HuntCombatant>& roster, CombatantId actor, const ActionRequest& action) const {
        DR_ASSERT_MSG(roster.Contains(actor), "Actor not in roster");
        DR_ASSERT_MSG(roster.Contains(action.Target), "Target not in roster");
        auto context = ResolutionContext{
            .ActorId = actor,
            .TargetId = action.Target,
            .Actor = roster.Get(actor),
            .Target = roster.Get(action.Target),
            .Context = action.Context
        };

        switch (action.Kind) {
            using enum ActionRequestKind;
            case Attack: return ResolveAttack(context);
            case Capture: return ResolveCapture(context);
            case Item: return ResolveItem(context, m_Inventory);
            case Flee: return ResolveFlee(context);
            case Defend: return ResolveDefend(context);
            case Hide: return ResolveHide(context);
            case LookPretty: return {};
            default: break;   
        }

        DR_ASSERT_MSG(false, "Unhandled action");
        return {};
    }
}