#include "CommonTest.h"

#include "Combat/CombatSchedule.h"

#include <span>
#include <vector>

namespace Combat {
	struct RRTest : public ::testing::Test {
        RoundRobinScheduler scheduler{};
        CombatantId c1{1};
        CombatantId c2{2};
        CombatantId c3{3};
	};

	TEST_F(RRTest, GetReadyCombatants_WithNoCombatants_ReturnsEmpty) { 
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
	}

	TEST_F(RRTest, GetReadyCombatants_WithOneCombatant_ReturnsCombatant) {
        scheduler.AddCombatant(c1, ZeroTime);

		auto next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], c1);
	}

	TEST_F(RRTest, GetReadyCombatants_WithTwoCombatants_SwitchesBetween) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);

		auto next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], c1);
        scheduler.CompleteAction(c1);

        next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], c2);
        scheduler.CompleteAction(c2);

        next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], c1);
	}

    TEST_F(RRTest, CompleteAction_WithWrongActor_DoesNothing) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);

        auto next = scheduler.GetReadyCombatants()[0];
        scheduler.CompleteAction({999});
        auto nextNext = scheduler.GetReadyCombatants()[0];

        ASSERT_EQ(next, nextNext);
    }

    TEST_F(RRTest, AddCombatant_AddsCombatant_BeforeCurrent) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);
        scheduler.AddCombatant(c3, ZeroTime);

        scheduler.CompleteAction(c1);
        scheduler.CompleteAction(c2);

        scheduler.AddCombatant(CombatantId{4}, ZeroTime);

        auto count = 0;
        while(scheduler.GetReadyCombatants()[0] != CombatantId{4}) {
            count++;
            scheduler.CompleteAction(scheduler.GetReadyCombatants()[0]);
        }

        ASSERT_EQ(count, 3);
    }

    TEST_F(RRTest, RemoveCombatant_FromBefore_MaintainsCurrent) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);
        scheduler.AddCombatant(c3, ZeroTime);
        
        scheduler.CompleteAction(c1);
        scheduler.RemoveCombatant(c1);

        ASSERT_EQ(scheduler.GetReadyCombatants()[0], c2);
    }

    TEST_F(RRTest, RemoveCombatant_FromCurrent_IncrementsCurrent) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);
        scheduler.AddCombatant(c3, ZeroTime);
        scheduler.CompleteAction(c1);
        scheduler.RemoveCombatant(c2);
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], c3);
    }

    TEST_F(RRTest, RemoveCombatant_FromCurrentAtEnd_IncrementsCurrent) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);
        scheduler.AddCombatant(c3, ZeroTime);
        scheduler.CompleteAction(c1);
        scheduler.CompleteAction(c2);
        scheduler.RemoveCombatant(c3);
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], c1);        
    }

    TEST_F(RRTest, RemoveCombatant_FromAfter_MaintainsCurrent) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.AddCombatant(c2, ZeroTime);
        scheduler.AddCombatant(c3, ZeroTime);
        scheduler.CompleteAction(c1);
        scheduler.RemoveCombatant(c3);
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], c2);
    }

    TEST_F(RRTest, RemoveCombatant_FromSingle_MakesEmpty) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.RemoveCombatant(c1);
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RRTest, RemoveCombatant_FromEmpty_DoesNothing) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.RemoveCombatant(c1);

        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }
}