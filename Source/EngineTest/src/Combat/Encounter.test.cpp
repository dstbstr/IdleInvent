#include "TestCombatTypes.h"
#include "Combat/CombatSchedule.h"
#include "Combat/Encounter.h"

#include "CommonTest.h"

#include <expected>

namespace Combat {

    struct EncounterTest : public ::testing::Test {
        
        TestCombatant P1 = {.Hp = 10, .Attack = 3};
        TestCombatant P2 = {.Hp = 5, .Attack = 2};
        Social::FactionId GoodGuys = Social::ToFactionId(TestFaction::GoodGuys);
        Social::FactionId BadGuys = Social::ToFactionId(TestFaction::BadGuys);

        TestEncounter MakeEncounter() { return TestEncounter(TestRules{}, std::make_unique<RoundRobinScheduler>()); }
    };

    TEST_F(EncounterTest, Roster_AfterAdd_ContainsCombatants) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

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
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

        auto ready = encounter.GetReadyCombatants();
        ASSERT_EQ(1, ready.size());
        ASSERT_EQ(p1Id, ready[0]);
    }

    TEST_F(EncounterTest, Submit_WhenFinished_Errors) {
        auto encounter = MakeEncounter();
        P1.Attack = P2.Hp;
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

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
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

        auto result = encounter.Submit(p2Id, {.Target = p1Id});
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), SubmitError::ActorNotReady);
    }

    TEST_F(EncounterTest, Submit_WhenDisabled_Errors) { 
        P1.Hp = 0;
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

        auto result = encounter.Submit(p1Id, {.Target = p2Id});
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), SubmitError::InvalidAction);
    }

    TEST_F(EncounterTest, Submit_WhenValid_AppliesRule) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

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

    TEST_F(EncounterTest, IsDisabled_WhenDead_ReturnsTrue) {
        P1.Hp = 0;
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);

        ASSERT_TRUE(encounter.IsDisabled(p1Id));
    }

    TEST_F(EncounterTest, SkipTurn_WithPlayer_ReturnsSkipped) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);

        auto events = encounter.SkipTurn(p1Id);
        ASSERT_EQ(1, events.size());
        ASSERT_EQ(events[0].Kind, TestEventKind::TurnSkipped);
    }

    TEST_F(EncounterTest, SkipTurn_WithPlayer_IncrementsProgress) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);

        auto progress = encounter.GetProgress();
        encounter.SkipTurn(p1Id);
        ASSERT_EQ(progress.CompletedTurns + 1, encounter.GetProgress().CompletedTurns);
        auto ready = encounter.GetReadyCombatants();
        ASSERT_EQ(p2Id, ready.front());
    }

    TEST_F(EncounterTest, SkipTurn_WithInvalid_DoesNothing) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);
        auto progress = encounter.GetProgress();
        auto events = encounter.SkipTurn(p2Id);
        ASSERT_EQ(0, events.size());
        ASSERT_EQ(progress.CompletedTurns, encounter.GetProgress().CompletedTurns);
    }

    TEST_F(EncounterTest, Update_WithElapsed_IncrementsProgress) {
        auto encounter = MakeEncounter();
        encounter.Update(OneSecond);

        auto progress = encounter.GetProgress();
        ASSERT_GE(progress.Elapsed, OneSecond);
    }

    TEST_F(EncounterTest, Find_WithExistingEntry_ReturnsMatch) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto p2Id = encounter.AddCombatant(BadGuys, P2, ZeroTime);
        auto result = encounter.Find([&](CombatantId id, Social::FactionId faction, const TestCombatant& combatant) {
            return faction == GoodGuys;
        });
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(*result, p1Id);
    }

    TEST_F(EncounterTest, Find_WithNoMatch_ReturnsNullopt) {
        auto encounter = MakeEncounter();
        auto p1Id = encounter.AddCombatant(GoodGuys, P1, ZeroTime);
        auto result = encounter.Find([&](CombatantId id, Social::FactionId faction, const TestCombatant& combatant) {
            return faction == BadGuys;
        });
        ASSERT_FALSE(result.has_value());
    }
} // namespace Combat
