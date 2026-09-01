#include "Combat/CombatSchedule.h"

#include "CommonTest.h"

#include <span>
#include <vector>

namespace Combat {
    struct RealTimeTest : public ::testing::Test {
        RealTimeScheduler scheduler{};
        CombatantId c1{1};
        CombatantId c2{2};
        CombatantId c3{3};
    };

    TEST_F(RealTimeTest, GetReadyCombatants_WithNoCombatants_ReturnsEmpty) {
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RealTimeTest, GetReadyCombatants_BeforeUpdate_ReturnsEmpty) {
        scheduler.AddCombatant(c1, OneSecond);

        auto next = scheduler.GetReadyCombatants();
        ASSERT_TRUE(next.empty());
    }

    TEST_F(RealTimeTest, GetReadyCombatants_AfterUpdate_ReturnsReady) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.AddCombatant(c2, OneMinute);

        scheduler.Update(OneSecond);

        auto next = scheduler.GetReadyCombatants();
        ASSERT_EQ(next.size(), 1);
        ASSERT_EQ(next[0], c1);
    }

    TEST_F(RealTimeTest, GetReadyCombatants_WhenMultipleReady_ReturnsMultiple) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.AddCombatant(c2, OneSecond);
        scheduler.Update(OneSecond);

        auto next = scheduler.GetReadyCombatants();
        
        ASSERT_EQ(next.size(), 2);
        ASSERT_NE(std::ranges::find(next, c1), next.end());
        ASSERT_NE(std::ranges::find(next, c2), next.end());
    }

    TEST_F(RealTimeTest, GetReadyCombatants_BeforeComplete_ReturnsSame) { 
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.Update(OneSecond);

        auto next1 = scheduler.GetReadyCombatants();
        scheduler.Update(OneSecond);
        auto next2 = scheduler.GetReadyCombatants();

        ASSERT_EQ(next1[0], next2[0]);
    }

    TEST_F(RealTimeTest, CompleteAction_WithWrongActor_DoesNothing) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.AddCombatant(c2, OneSecond);
        scheduler.Update(OneSecond);

        auto next = scheduler.GetReadyCombatants()[0];
        scheduler.CompleteAction({999});
        auto nextNext = scheduler.GetReadyCombatants()[0];

        ASSERT_EQ(next, nextNext);
    }

    TEST_F(RealTimeTest, AddCombatant_AddsCombatant_WithFullTime) {
        scheduler.AddCombatant(c1, OneSecond * 2);
        scheduler.Update(OneSecond);
        ASSERT_EQ(0, scheduler.GetReadyCombatants().size());
        scheduler.Update(OneSecond);
        ASSERT_EQ(1, scheduler.GetReadyCombatants().size());
    }

    TEST_F(RealTimeTest, Update_MoreThanInterval_SavesExtra) { 
        scheduler.AddCombatant(c1, OneSecond * 3);
        scheduler.Update(OneSecond * 4);
        ASSERT_EQ(1, scheduler.GetReadyCombatants().size());
        
        scheduler.CompleteAction(c1);
        ASSERT_EQ(0, scheduler.GetReadyCombatants().size());

        scheduler.Update(OneSecond * 2);
        ASSERT_EQ(1, scheduler.GetReadyCombatants().size());
    }

    TEST_F(RealTimeTest, RemoveCombatant_BeforeReady_MakesEmpty) { 
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.RemoveCombatant(c1);
        scheduler.Update(OneSecond);

        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RealTimeTest, RemoveCombatant_WhenReady_MakesEmpty) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.Update(OneSecond);

        scheduler.RemoveCombatant(c1);
        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RealTimeTest, RemoveCombatant_FromEmpty_DoesNothing) {
        scheduler.RemoveCombatant(c1);

        ASSERT_TRUE(scheduler.GetReadyCombatants().empty());
    }

    TEST_F(RealTimeTest, ApplySpeedModifier_Haste_SpeedsUpReady) {
        scheduler.AddCombatant(c1, OneSecond * 2);
        scheduler.ApplySpeedModifier(c1, Modifier{.Mul = 2.f});
        scheduler.Update(OneSecond);
        ASSERT_EQ(1, scheduler.GetReadyCombatants().size());
    }

    TEST_F(RealTimeTest, ApplySpeedModifier_Slow_SlowsDownReady) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.ApplySpeedModifier(c1, Modifier{.Mul = 0.5f});
        scheduler.Update(OneSecond);
        ASSERT_EQ(0, scheduler.GetReadyCombatants().size());
    }

    TEST_F(RealTimeTest, ApplySpeedModifier_Freeze_StopsReady) {
        scheduler.AddCombatant(c1, OneSecond);
        scheduler.ApplySpeedModifier(c1, Modifier{.Mul = 0.f});
        scheduler.Update(OneMinute);
        ASSERT_EQ(0, scheduler.GetReadyCombatants().size());
    }

    TEST_F(RealTimeTest, AddCombatant_ZeroInterval_DoesNothing) {
        scheduler.AddCombatant(c1, ZeroTime);
        scheduler.Update(OneSecond);
        ASSERT_EQ(0, scheduler.GetReadyCombatants().size());
    }
} // namespace Combat