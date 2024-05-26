#include "CommonTest.h"

#include "InventLib/Mechanics/Advancement.h"
#include "InventLib/Mechanics/Progression.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"


namespace Invent {
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

		Advancement advancement{ "Test", Progression{}, AdvancementCosts::Linear<10> };

		AdvancementListener listener;
	};

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotChangeLevel) {
		advancement.Tick();

		ASSERT_TRUE(listener.Events.empty());
	}

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotIncreaseLevelCost) {
		auto original = advancement.ExpToNextLevel;
		advancement.Tick();

		ASSERT_EQ(original, 10);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_PublishesLevel) {
		advancement.Progress.BaseProgress = advancement.ExpToNextLevel;
		advancement.Tick();

		ASSERT_EQ(listener.Events.size(), 1);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_ResetsProgress) {
		advancement.Progress.BaseProgress = advancement.ExpToNextLevel;
		advancement.Tick();

		ASSERT_EQ(advancement.CurrentExp, 0);
	}

	TEST_F(AdvancementTest, Tick_WithOneAndAHalfLevels_KeepsTheChange) {
		auto tickAmount = static_cast<size_t>(advancement.ExpToNextLevel * 1.5F);
		advancement.Progress.BaseProgress = tickAmount;
		auto changeAmount = tickAmount - advancement.ExpToNextLevel;
		advancement.Tick();

		ASSERT_EQ(advancement.CurrentExp, changeAmount );
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_IncreasesNextLevelCost) {
		advancement.Progress.BaseProgress = advancement.ExpToNextLevel;
		advancement.Tick();

		ASSERT_EQ(advancement.ExpToNextLevel, 20); // 10 base + 10 linear growth
	}

	TEST_F(AdvancementTest, Tick_WithMultipleLevelsProgress_PublishesMultipleEvents) {
		advancement.Progress.BaseProgress = advancement.ExpToNextLevel * 100;
		advancement.Tick();

		ASSERT_TRUE(listener.Events.size() > 2);
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