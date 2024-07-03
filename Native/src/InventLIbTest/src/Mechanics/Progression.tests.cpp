#include "InventLib/Mechanics/Progression.h"

#include "CommonTest.h"

namespace Invent {
    struct ProgressionTest : public ::testing::Test {
        Progression progression{};
    };

    TEST_F(ProgressionTest, GetProgress_AfterOneSecond_ReturnsOne) {
        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(1, progress);
    }

    TEST_F(ProgressionTest, GetProgress_WithMultiplier_ReturnsMultipliedValue) {
        progression.AddModifier({0, 2.0f, OneYear});

        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(2, progress);
    }

    TEST_F(ProgressionTest, GetProgress_WithTwoMultipliers_ReturnsMultipliedValue) {
        progression.AddModifier({0, 3.0f, OneYear});
        progression.AddModifier({0, 3.0f, OneYear});

        auto progress = progression.GetProgress(OneSecond);
        ASSERT_EQ(9, progress);
    }

    TEST_F(ProgressionTest, CopyAssignment_WithMultipliers_CopiesMultipliers) {
        progression.AddModifier({0, 3.0f, OneYear});
        auto copy = progression;

        ASSERT_EQ(progression.GetProgress(OneSecond), copy.GetProgress(OneSecond));
    }

    TEST_F(ProgressionTest, CopyConstructor_WithMultipliers_CopiesMultipliers) {
        progression.AddModifier({0, 3.0f, OneYear});
        Progression copy{progression};

        ASSERT_EQ(progression.GetProgress(OneSecond), copy.GetProgress(OneSecond));
    }

    struct ProgressionLoadTest : public ::testing::Test {
        ProgressionSave save;

        ModifierSave MakeTemp() {
            ModifierSave result{};
            result.Add = 0;
            result.Mul = 2'000; // 2 in fixed point
            result.DurationInYears = 10;
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
        save.TempCount = 1;
        save.Temporary[0] = MakeTemp();
        Progression progression{save};
        ASSERT_EQ(2, progression.GetProgress(OneSecond));
    }

    TEST_F(ProgressionLoadTest, Load_WithMultipleTemporary_AddsAll) {
        save.TempCount = 2;
        save.Temporary[0] = MakeTemp();
        save.Temporary[1] = MakeTemp();
        Progression progression{save};
        ASSERT_EQ(4, progression.GetProgress(OneSecond));
    }

    TEST_F(ProgressionLoadTest, Load_WithMultipleTemporaryAndPermanent_AddsAll) {
        save.TempCount = 2;
        save.Permanent.Add = 1;
        save.Permanent.Mul = 2'000; // 2 in fixed point
        save.Temporary[0] = MakeTemp();
        save.Temporary[1] = MakeTemp();
        Progression progression{save};
        ASSERT_EQ(8, progression.GetProgress(OneSecond));
    }

    struct ProgressionSaveTest : public ::testing::Test {
        ProgressionSave save;
        Progression progression;
    };

    TEST_F(ProgressionSaveTest, Save_WithPermanent_SavesPermanent) {
        progression.Save(save);
        ASSERT_EQ(1, save.Permanent.Add);
        ASSERT_EQ(1'000, save.Permanent.Mul);
    }

    TEST_F(ProgressionSaveTest, Save_WithOneTemp_SavesTemp) {
        progression.AddModifier({1, 2.0f, OneGameYear});
        progression.Save(save);
        ASSERT_EQ(1, save.TempCount);
        ASSERT_EQ(1, save.Temporary[0].Add);
        ASSERT_EQ(2'000, save.Temporary[0].Mul);
        ASSERT_EQ(1, save.Temporary[0].DurationInYears);
    }

    TEST_F(ProgressionSaveTest, Save_WithTwoTemps_SavesTemps) {
        progression.AddModifier({1, 2.0f, OneGameYear});
        progression.AddModifier({2, 3.0f, OneGameYear * 2});
        progression.Save(save);

        ASSERT_EQ(2, save.TempCount);
        ASSERT_EQ(1, save.Temporary[0].Add);
        ASSERT_EQ(2'000, save.Temporary[0].Mul);
        ASSERT_EQ(1, save.Temporary[0].DurationInYears);

        ASSERT_EQ(2, save.Temporary[1].Add);
        ASSERT_EQ(3'000, save.Temporary[1].Mul);
        ASSERT_EQ(2, save.Temporary[1].DurationInYears);
    }
} // namespace Invent