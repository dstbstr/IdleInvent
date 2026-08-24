#include "Combat/CombatSchedule.h"
#include "Combat/Encounter.h"

#include "CommonTest.h"

#include <expected>

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
        bool CanAct(const Roster<TestCombatant>& roster, CombatantId actor) const {
            return roster.Contains(actor) && roster.Get(actor).Hp > 0;
        }

        bool CanSubmit(const Roster<TestCombatant>& roster, CombatantId actor, const TestAction& action) {
            return CanAct(roster, actor) && 
                roster.Contains(action.Target) && 
                roster.Get(action.Target).Hp > 0 &&
                actor != action.Target;
        }

        ActionResolution<TestEvent> Update(Roster<TestCombatant>& roster, BaseTime elapsed) const {
            return {};
        }

        ActionResolution<TestEvent> EndTurn(Roster<TestCombatant>& roster, CombatantId actor, TurnEnd endKind, const EncounterProgress& progress) const {
            return {
                .Events = {{
                    .Kind = endKind == TurnEnd::Skipped ? TestEventKind::TurnSkipped : TestEventKind::TurnEnded,
                    .Actor = actor
                }}
            };
        }

        ActionResolution<TestEvent> Resolve(Roster<TestCombatant>& roster, CombatantId actor, const TestAction& action) const {
            auto& attacker = roster.Get(actor);
            auto& defender = roster.Get(action.Target);
            auto damage = attacker.Attack;

            defender.Hp -= damage;

            ActionResolution<TestEvent> result{
                .Events = {{
                    .Kind = TestEventKind::Damaged, 
                    .Actor = actor, 
                    .Target = action.Target, 
                    .Amount = damage
                }}
            };

            if(defender.Hp <= 0) {
                result.Events.push_back({
                    .Kind = TestEventKind::Defeated,
                    .Actor = actor,
                    .Target = action.Target
                });
                result.Events.push_back({.Kind = TestEventKind::CombatEnded});
                result.EncounterFinished = true;
            }

            return result;
        }
    };

    struct EncounterTest : public ::testing::Test {
        using TEncounter = Encounter<TestCombatant, TestAction, TestEvent, TestRules>;
        
        TestCombatant P1 = {.Hp = 10, .Attack = 3};
        TestCombatant P2 = {.Hp = 5, .Attack = 2};
        Social::FactionId GoodGuys = Social::ToFactionId(TestFaction::GoodGuys);
        Social::FactionId BadGuys = Social::ToFactionId(TestFaction::BadGuys);

        TEncounter MakeEncounter() { return TEncounter(TestRules{}, std::make_unique<RoundRobinScheduler>()); }
    };

    TEST_F(EncounterTest, Roster_AfterAdd_ContainsCombatants) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        ASSERT_TRUE(p1Id.IsValid());
        ASSERT_TRUE(p2Id.IsValid());
        ASSERT_NE(p1Id, p2Id);

        const auto& roster = encounter.GetRoster();
        ASSERT_EQ(2, roster.Size());

        ASSERT_TRUE(roster.Contains(p1Id));
        ASSERT_TRUE(roster.Contains(p2Id));

        ASSERT_EQ(roster.GetFaction(p1Id), GoodGuys);
        ASSERT_EQ(roster.GetFaction(p2Id), BadGuys);
    }

    TEST_F(EncounterTest, Ready_AfterAdd_ContainsFirstCombatant) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        auto ready = encounter.GetReadyCombatants();
        ASSERT_EQ(1, ready.size());
        ASSERT_EQ(p1Id, ready[0]);
    }

    TEST_F(EncounterTest, Submit_WhenFinished_Errors) {
        auto encounter = MakeEncounter();
        P1.Attack = P2.Hp;
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        auto result = encounter.Submit(p1Id, {.Target = p2Id});
        ASSERT_TRUE(result.has_value());
        auto events = *result;
        auto it = std::find_if(events.begin(), events.end(), [](const auto& e) {
            return e.Kind == TestEventKind::CombatEnded;
        });
        ASSERT_NE(it, events.end());

        result = encounter.Submit(p2Id, {.Target = p1Id});
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), SubmitError::EncounterFinished);
    }

    TEST_F(EncounterTest, Submit_WhenInactive_Errors) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        auto result = encounter.Submit(p2Id, {.Target = p1Id});
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), SubmitError::ActorNotReady);
    }

    TEST_F(EncounterTest, Submit_WhenCannotAct_Errors) { 
        P1.Hp = 0;
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        auto result = encounter.Submit(p1Id, {.Target = p2Id});
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), SubmitError::InvalidAction);
    }

    TEST_F(EncounterTest, Submit_WhenValid_AppliesRule) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);
        auto p2Id = encounter.AddCombatant(BadGuys, P2);

        auto result = encounter.Submit(p1Id, {.Target = p2Id});
        ASSERT_TRUE(result.has_value());
        auto events = *result;
        ASSERT_EQ(2, events.size());
        ASSERT_TRUE(events[0].Kind == TestEventKind::Damaged || events[1].Kind == TestEventKind::Damaged);
        ASSERT_TRUE(events[0].Kind == TestEventKind::TurnEnded || events[1].Kind == TestEventKind::TurnEnded);

        auto p2 = encounter.GetRoster().Get(p2Id);
        ASSERT_EQ(p2.Hp, P2.Hp - P1.Attack);

        const auto& progress = encounter.GetProgress();
        ASSERT_EQ(progress.CompletedTurns, 1);
    }

    TEST_F(EncounterTest, CanAct_WhenDead_ReturnsFalse) {
        P1.Hp = 0;
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);

        ASSERT_FALSE(encounter.CanAct(p1Id));
    }

    TEST_F(EncounterTest, SkipTurn_WithPlayer_ReturnsSkipped) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1);

        auto events = encounter.SkipTurn(p1Id);
        ASSERT_EQ(1, events.size());
        ASSERT_EQ(events[0].Kind, TestEventKind::TurnSkipped);
    }
} // namespace Combat
