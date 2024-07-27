#include "CommonTest.h"

#include "InventLib/Mechanics/Advancement.h"
#include "InventLib/Mechanics/Progression.h"
#include "InventLib/Mechanics/ScaleFunc.h"

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
		
		Advancement advancement{"Test", 10, Scale::Linear<int, 10>, 0 };

		void SetProgress(s64 add, f32 mul) {
            advancement.Progress.ClearModifiers();
            advancement.Progress.AddPermanent(Modifier{.Add = add, .Mul = mul});
		}

		AdvancementListener listener;
	};

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotChangeLevel) {
		advancement.Tick(OneSecond);

		ASSERT_TRUE(listener.Events.empty());
	}

	TEST_F(AdvancementTest, Tick_WithNoProgress_DoesNotIncreaseLevelCost) {
		auto original = advancement.ExpToNextLevel;
		advancement.Tick(OneSecond);

		ASSERT_EQ(original, 10);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_PublishesLevel) {
        SetProgress(advancement.ExpToNextLevel, 1);
		advancement.Tick(OneSecond);

		ASSERT_EQ(listener.Events.size(), 1);
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_ResetsProgress) {
        SetProgress(advancement.ExpToNextLevel, 1);
		advancement.Tick(OneSecond);

		ASSERT_EQ(advancement.CurrentExp, 0);
	}

	TEST_F(AdvancementTest, Tick_WithOneAndAHalfLevels_KeepsTheChange) {
        auto tickAmount = static_cast<size_t>(advancement.ExpToNextLevel * 1.5F);
		SetProgress(tickAmount, 1);
		auto changeAmount = tickAmount - advancement.ExpToNextLevel;
		advancement.Tick(OneSecond);

		ASSERT_EQ(advancement.CurrentExp, changeAmount );
	}

	TEST_F(AdvancementTest, Tick_WithOneLevelProgress_IncreasesNextLevelCost) {
		SetProgress(advancement.ExpToNextLevel, 1);
		advancement.Tick(OneSecond);

		ASSERT_EQ(advancement.ExpToNextLevel, 20); // 10 base + 10 linear growth
	}

	TEST_F(AdvancementTest, Tick_WithMultipleLevelsProgress_PublishesMultipleEvents) {
		SetProgress(advancement.ExpToNextLevel, 100);
		advancement.Tick(OneSecond);

		ASSERT_TRUE(listener.Events.size() > 2);
	}
}