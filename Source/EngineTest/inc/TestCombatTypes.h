#pragma once

#include "Combat/CombatSchedule.h"
#include "Combat/CombatController.h"
#include "Combat/Encounter.h"

#include <utility>
#include <optional>

namespace Combat {
    struct TestCombatant {
        s32 Hp{};
        s32 Attack{};
    };

    struct TestAction {
        CombatantId Target{};
    };

    enum struct TestEventKind : u8 { Damaged, Defeated, CombatEnded, TurnEnded, TurnSkipped };
    enum struct TestFaction : u8 { GoodGuys, BadGuys };

    struct TestEvent {
        TestEventKind Kind{};
        CombatantId Actor{};
        CombatantId Target{};
        s32 Amount{};
    };

    struct TestRules {
        bool IsDisabled(const Roster<TestCombatant>& roster, CombatantId actor) const {
            return !roster.Contains(actor) || roster.Get(actor).Hp <= 0;
        }

        bool CanSubmit(const Roster<TestCombatant>& roster, CombatantId actor, const TestAction& action) const {
            return !IsDisabled(roster, actor) && 
                roster.Contains(action.Target) && 
                roster.Get(action.Target).Hp > 0 &&
                actor != action.Target;
        }

        ActionResolution<TestEvent> Update(Roster<TestCombatant>& roster, BaseTime elapsed) const { return {}; }

        ActionResolution<TestEvent> EndTurn(
            Roster<TestCombatant>& roster, CombatantId actor, TurnEnd endKind, const EncounterProgress& progress
        ) const {
            return {
                .Events = {
                    {.Kind = endKind == TurnEnd::Skipped ? TestEventKind::TurnSkipped : TestEventKind::TurnEnded,
                     .Actor = actor}
                }
            };
        }

        ActionResolution<TestEvent>
        Resolve(Roster<TestCombatant>& roster, CombatantId actor, const TestAction& action) const {
            auto& attacker = roster.Get(actor);
            auto& defender = roster.Get(action.Target);
            auto damage = attacker.Attack;

            defender.Hp -= damage;

            ActionResolution<TestEvent> result{
                .Events = {{.Kind = TestEventKind::Damaged, .Actor = actor, .Target = action.Target, .Amount = damage}}
            };

            if(defender.Hp <= 0) {
                result.Events.push_back({.Kind = TestEventKind::Defeated, .Actor = actor, .Target = action.Target});
                result.Events.push_back({.Kind = TestEventKind::CombatEnded});
                result.EncounterFinished = true;
            }

            return result;
        }
    };

    using TestEncounter = Encounter<TestCombatant, TestAction, TestEvent, TestRules>;
    struct TestController : public ICombatController<TestAction, TestEncounter> {
        std::optional<TestAction> NextAction{};
        bool Repeat{false};
        size_t Calls{0};

        std::optional<TestAction> GetNextAction(CombatantId, const TestEncounter&) override { 
            ++Calls;
            if(Repeat) return NextAction;

            return std::exchange(NextAction, std::nullopt);
        }
    };
}