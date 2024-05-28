#include "CommonTest.h"

#include "InventLib/Mechanics/Advancement.h"
#include "InventLib/Mechanics/Progression.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"


namespace Invent {
    using namespace std::chrono_literals;

	struct AdvancementListener {
		AdvancementListener() {
			ServiceLocator::Get().GetOrCreate<PubSub<Advancement>>().Subscribe([this](const Advancement& advancement) {
				Events.push_back(advancement);
			});
		}

		std::vector<Advancement> Events;
	};

	struct AdvancementTest : testing::Test {
		void SetUp() override {
			ServiceLocator::Get().CreateIfMissing<PubSub<Advancement>>();
		}

		void TearDown() override {
			ServiceLocator::Get().ResetAll();
		}

		Advancement advancement{ "Test", AdvancementCosts::Linear<10> };

		void SetProgress(s64 add, f32 mul, bool active) {
			advancement.Progress.Modifiers.clear();
            advancement.Progress.Modifiers.push_back(Modifier{.Add = add, .Mul = mul});
            advancement.Progress.Active = active;
		}

		AdvancementListener listener;
	};

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotChangeLevel) {
		advancement.Tick(1s);

		ASSERT_TRUE(listener.Events.empty());
	}

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotIncreaseLevelCost) {
		auto original = advancement.ExpToNextLevel;
		advancement.Tick(1s);

		ASSERT_EQ(original, 10);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_PublishesLevel) {
        SetProgress(advancement.ExpToNextLevel, 1, true);
		advancement.Tick(1s);

		ASSERT_EQ(listener.Events.size(), 1);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_ResetsProgress) {
        SetProgress(advancement.ExpToNextLevel, 1, true);
		advancement.Tick(1s);

		ASSERT_EQ(advancement.CurrentExp, 0);
	}

	TEST_F(AdvancementTest, Tick_WithOneAndAHalfLevels_KeepsTheChange) {
        auto tickAmount = static_cast<size_t>(advancement.ExpToNextLevel * 1.5F);
		SetProgress(tickAmount, 1, true);
		auto changeAmount = tickAmount - advancement.ExpToNextLevel;
		advancement.Tick(1s);

		ASSERT_EQ(advancement.CurrentExp, changeAmount );
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_IncreasesNextLevelCost) {
		SetProgress(advancement.ExpToNextLevel, 1, true);
		advancement.Tick(1s);

		ASSERT_EQ(advancement.ExpToNextLevel, 20); // 10 base + 10 linear growth
	}

	TEST_F(AdvancementTest, Tick_WithMultipleLevelsProgress_PublishesMultipleEvents) {
		SetProgress(advancement.ExpToNextLevel, 100, true);
		advancement.Tick(1s);

		ASSERT_TRUE(listener.Events.size() > 2);
	}

	TEST_F(AdvancementTest, Tick_WithDisabledProgress_DoesNotLevel) {
		SetProgress(advancement.ExpToNextLevel, 1, false);
		advancement.Tick(1s);

		ASSERT_EQ(advancement.CurrentLevel, 1);
	}

	TEST(AdvancementCosts, Constant_WithAnyLevel_ReturnsConstant) {
		auto costs = AdvancementCosts::Constant<10>;
		ASSERT_EQ(costs(0), 10);
		ASSERT_EQ(costs(10), 10);
		ASSERT_EQ(costs(200), 10);
	}

	TEST(AdvancementCosts, Linear_WithBase10AndLevel3_Returns30) {
		ASSERT_EQ(AdvancementCosts::Linear<10>(3), 30);
	}

	TEST(AdvancementCosts, Logarithmic_WithBase2AndLevel3_Returns8) {
		ASSERT_EQ(AdvancementCosts::Logarithmic<2>(3), 8);
	}

	TEST(AdvancementCosts, Logarithmic_WithBase10AndLevel3_Returns1000) {
		ASSERT_EQ(AdvancementCosts::Logarithmic<10>(3), 1000);
	}

	TEST(AdvancementCosts, Punctuated_WithStep2AndLinearBase10AndLevel3_Returns20) {
		std::function<size_t(size_t)> costs = AdvancementCosts::MakePunctuated<2>(AdvancementCosts::Linear<10>);
		ASSERT_EQ(costs(3), 20); // 10, 10, 20, 20, 30, 30
	}
}