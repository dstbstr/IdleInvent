#include "CommonTest.h"
#include "InventLib/Achievements/Achievements.h"
#include "Core/DesignPatterns/PubSub.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Mechanics/Unlockable.h"

namespace Invent {
	struct AchievementListener {
		AchievementListener() {
			handle = ServiceLocator::Get().GetRequired<PubSub<Achievement>>().Subscribe([this](const Achievement& achievement) {
				ReceivedAchievements.push_back(achievement);
			});
		}
		~AchievementListener() {
			ServiceLocator::Get().GetRequired<PubSub<Achievement>>().Unsubscribe(handle);
		}

		std::vector<Achievement> ReceivedAchievements;
		size_t handle{ 0 };
	};

	struct AchievementsTest : public ::testing::Test {
		static void SetUpTestSuite() {
			Achievements::Initialize();
		}

		static void TearDownTestSuite() {
			ServiceLocator::Get().ResetAll();
		}

		void SetUp() override {
			gameState = &ServiceLocator::Get().GetOrCreate<GameState>();
		}

		GameState* gameState{ nullptr };
		AchievementListener listener;
	};

	TEST_F(AchievementsTest, TimeAchievement_AfterOneSecond_DoesNotUnlock) {
		gameState->ElapsedTicks = 1;
		Unlockables::Tick();

		ASSERT_TRUE(listener.ReceivedAchievements.empty());
	}

	TEST_F(AchievementsTest, TimeAchievement_AfterOneMinute_Unlocks) {
		ASSERT_TRUE(listener.ReceivedAchievements.empty());
		gameState->ElapsedTicks = 60;
		Unlockables::Tick();

		ASSERT_EQ(listener.ReceivedAchievements.size(), 1);
	}

	TEST_F(AchievementsTest, TimeAchievement_AfterOneHour_UnlocksOneAtATime) {
		ASSERT_TRUE(listener.ReceivedAchievements.empty());
		gameState->ElapsedTicks = 3600;
		
		Unlockables::Tick();
		ASSERT_EQ(listener.ReceivedAchievements.size(), 1);
		
		Unlockables::Tick();
		ASSERT_EQ(listener.ReceivedAchievements.size(), 2);
	}
}