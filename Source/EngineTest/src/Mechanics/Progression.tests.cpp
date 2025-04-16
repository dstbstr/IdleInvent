#include "CommonTest.h"

#include "Mechanics/Progression.h"

namespace Invent {
    struct ProgressionTest : public ::testing::Test {
        Progression progression{};
        void SetUp() override {
            progression.AddPermanent({.Add = 1, .Mul = 1.0f});
        }
    };

    TEST_F(ProgressionTest, GetProgress_AfterOneSecond_ReturnsOne) {
        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(1, progress);
    }

    TEST_F(ProgressionTest, GetProgress_WithMultiplier_ReturnsMultipliedValue) {
        progression.AddTemp({0, 2.0f}, OneYear);

        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(2, progress);
    }

    TEST_F(ProgressionTest, GetProgress_WithTwoMultipliers_ReturnsMultipliedValue) {
        progression.AddTemp({0, 3.0f}, OneYear);
        progression.AddTemp({0, 3.0f}, OneYear);

        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(9, progress);
    }

    TEST_F(ProgressionTest, CopyAssignment_WithMultipliers_CopiesMultipliers) {
        progression.AddTemp({0, 3.0f}, OneYear);
        auto copy = progression;

        ASSERT_EQ(progression.GetProgress(OneSecond), copy.GetProgress(OneSecond));
    }

    TEST_F(ProgressionTest, CopyConstructor_WithMultipliers_CopiesMultipliers) {
        progression.AddTemp({0, 3.0f}, OneYear);
        Progression copy{progression};

        ASSERT_EQ(progression.GetProgress(OneSecond), copy.GetProgress(OneSecond));
    }

    struct ProgressionLoadTest : public ::testing::Test {
        ProgressionSave save;

        ModifierSave MakeTemp() {
            ModifierSave result{};
            result.Add = 0;
            result.Mul = 2'000; // 2 in fixed point
            return result;
        }
    };

    TEST_F(ProgressionLoadTest, Load_WithPermanent_AddsPermanent) {
        save.Permanent.Add = 1;
        save.Permanent.Mul = 2'000; // 2 in fixed point
        Progression progression{save};
        ASSERT_EQ(2, progression.GetProgress(OneSecond));
    }

    TEST_F(ProgressionLoadTest, Load_WithTemporary_AddsTemporary) {
        save.Permanent = {1, 1'000};
        save.TempCount = 1;
        save.Temp[0] = MakeTemp();
        save.Durations[0] = 100;
        Progression progression{save};
        ASSERT_EQ(2, progression.GetProgress(OneSecond));
    }

    TEST_F(ProgressionLoadTest, Load_WithMultipleTemporary_AddsAll) {
        save.Permanent = {1, 1'000};
        save.TempCount = 2;
        save.Temp[0] = MakeTemp();
        save.Temp[1] = MakeTemp();
        save.Durations[0] = 100;
        save.Durations[1] = 100;
        Progression progression{save};
        ASSERT_EQ(4, progression.GetProgress(OneSecond));
    }

    TEST_F(ProgressionLoadTest, Load_WithMultipleTemporaryAndPermanent_AddsAll) {
        save.Permanent = {1, 2'000};
        save.TempCount = 2;
        save.Temp[0] = MakeTemp();
        save.Temp[1] = MakeTemp();
        save.Durations[0] = 100;
        save.Durations[1] = 100;
        Progression progression{save};
        ASSERT_EQ(8, progression.GetProgress(OneSecond));
    }

    struct ProgressionSaveTest : public ::testing::Test {
        ProgressionSave save;
        Progression progression;
        void SetUp() override {
			progression.AddPermanent({1, 1.0f});
		}
    };

    TEST_F(ProgressionSaveTest, Save_WithPermanent_SavesPermanent) {
        progression.Save(save);
        ASSERT_EQ(1, save.Permanent.Add);
        ASSERT_EQ(1'000, save.Permanent.Mul);
    }

    TEST_F(ProgressionSaveTest, Save_WithOneTemp_SavesTemp) {
        progression.AddTemp({1, 2.0f}, OneHour);
        progression.Save(save);
        ASSERT_EQ(1, save.TempCount);
        ASSERT_EQ(1, save.Temp[0].Add);
        ASSERT_EQ(2'000, save.Temp[0].Mul);
        ASSERT_EQ(60, save.Durations[0]);
    }

    TEST_F(ProgressionSaveTest, Save_WithTwoTemps_SavesTemps) {
        progression.AddTemp({1, 2.0f}, OneHour);
        progression.AddTemp({2, 3.0f}, OneHour * 2);
        progression.Save(save);

        ASSERT_EQ(2, save.TempCount);
        ASSERT_EQ(1, save.Temp[0].Add);
        ASSERT_EQ(2'000, save.Temp[0].Mul);
        ASSERT_EQ(60, save.Durations[0]);

        ASSERT_EQ(2, save.Temp[1].Add);
        ASSERT_EQ(3'000, save.Temp[1].Mul);
        ASSERT_EQ(120, save.Durations[1]);
    }
} // namespace Invent