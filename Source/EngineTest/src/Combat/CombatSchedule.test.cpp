#include "CommonTest.h"

#include "Combat/CombatSchedule.h"

#include <span>
#include <vector>

namespace Combat {
	struct RRTest : public ::testing::Test {
        RoundRobinScheduler scheduler{};
        std::vector<CombatantId> oneCombatant{{1}};
        std::vector<CombatantId> twoCombatants{{1}, {2}};
        std::vector<CombatantId> threeCombatants{{1}, {2}, {3}};
	};

	TEST_F(RRTest, GetReadyCombatants_WithNoCombatants_ReturnsEmpty) { 
		scheduler.Start({});
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
	}

	TEST_F(RRTest, GetReadyCombatants_WithOneCombatant_ReturnsCombatant) {
		scheduler.Start(std::span(oneCombatant));

		auto next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], oneCombatant[0]);
	}

	TEST_F(RRTest, GetReadyCombatants_WithTwoCombatants_SwitchesBetween) {
        scheduler.Start(std::span(twoCombatants));

		auto next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], twoCombatants[0]);
        scheduler.CompleteAction(twoCombatants[0]);

        next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], twoCombatants[1]);
        scheduler.CompleteAction(twoCombatants[1]);

        next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], twoCombatants[0]);
	}

    TEST_F(RRTest, CompleteAction_WithWrongActor_DoesNothing) {
        scheduler.Start(std::span(twoCombatants));

        auto next = scheduler.GetReadyCombatants()[0];
        scheduler.CompleteAction({999});
        auto nextNext = scheduler.GetReadyCombatants()[0];

        ASSERT_EQ(next, nextNext);
    }

    TEST_F(RRTest, AddCombatant_AddsCombatant_BeforeCurrent) {
        scheduler.Start(std::span(threeCombatants));

        scheduler.CompleteAction({1});
        scheduler.CompleteAction({2});

        scheduler.AddCombatant({4});

        auto count = 0;
        while(scheduler.GetReadyCombatants()[0] != CombatantId{4}) {
            count++;
            scheduler.CompleteAction(scheduler.GetReadyCombatants()[0]);
        }

        ASSERT_EQ(count, 3);
    }

    TEST_F(RRTest, RemoveCombatant_FromBefore_MaintainsCurrent) {
        scheduler.Start(std::span(threeCombatants));
        
        scheduler.CompleteAction({1});
        scheduler.RemoveCombatant({1});

        ASSERT_EQ(scheduler.GetReadyCombatants()[0], CombatantId{2});
    }

    TEST_F(RRTest, RemoveCombatant_FromCurrent_IncrementsCurrent) {
        scheduler.Start(std::span(threeCombatants));
        scheduler.CompleteAction({1});
        scheduler.RemoveCombatant({2});
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], CombatantId{3});
    }

    TEST_F(RRTest, RemoveCombatant_FromCurrentAtEnd_IncrementsCurrent) {
        scheduler.Start(std::span(threeCombatants));
        scheduler.CompleteAction({1});
        scheduler.CompleteAction({2});
        scheduler.RemoveCombatant({3});
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], CombatantId{1});        
    }

    TEST_F(RRTest, RemoveCombatant_FromAfter_MaintainsCurrent) {
        scheduler.Start(std::span(threeCombatants));
        scheduler.CompleteAction({1});
        scheduler.RemoveCombatant({3});
        ASSERT_EQ(scheduler.GetReadyCombatants()[0], CombatantId{2});
    }

    TEST_F(RRTest, RemoveCombatant_FromSingle_MakesEmpty) {
        scheduler.Start(std::span(oneCombatant));
        scheduler.RemoveCombatant({1});
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RRTest, RemoveCombatant_FromEmpty_DoesNothing) {
        scheduler.Start({});
        scheduler.RemoveCombatant({1});

        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }
}