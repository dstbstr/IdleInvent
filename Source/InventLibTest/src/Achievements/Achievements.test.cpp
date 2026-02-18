#include "CommonTest.h"
#include "TestGameState.h"

#include "InventLib/Achievements/Achievements.h"

#include <GameState/GameState.h>
#include <GameState/GameTime.h>
#include <Mechanics/Effect.h>
#include <Mechanics/Unlockable.h>
#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>

namespace Invent {
    struct AchievementListener {
        AchievementListener() {
            handle = ServiceLocator::Get().GetOrCreate<PubSub<Achievement>>().Subscribe(
                [this](const Achievement& achievement) { ReceivedAchievements.push_back(achievement); }
            );
        }

        std::vector<Achievement> ReceivedAchievements;
        std::optional<ScopedHandle> handle;
    };

    struct AchievementsTest : public ::testing::Test {
        static void TearDownTestSuite() { ServiceLocator::Get().ResetAll(); }

        void SetUp() override {
            Achievements::Initialize();
            Achievements::SaveState save{};
            Achievements::Load(save);

            GenerateTestGameState();
            auto& services = ServiceLocator::Get();
            gameState = &services.GetOrCreate<GameState>();
            unlockables = &services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
            services.CreateIfMissing<PubSub<std::vector<Effect>>>();
        }

        void TearDown() override { unlockables->clear(); }

        GameState* gameState{nullptr};
        std::unordered_map<std::string, Unlockable>* unlockables;
        AchievementListener listener;
    };

    TEST_F(AchievementsTest, Unlockables_AfterInitialize_ContainsAllTimeAchievements) {
        for(const auto& achievement: Achievements::Time) {
            ASSERT_TRUE(unlockables->contains(achievement.Name)) << achievement.Name;
        }
    }

    TEST_F(AchievementsTest, TimeAchievement_AfterOneSecond_DoesNotUnlock) {
        gameState->TimeElapsed = OneSecond;
        Unlockables::Tick();

        ASSERT_TRUE(listener.ReceivedAchievements.empty());
    }

    TEST_F(AchievementsTest, TimeAchievement_AfterOneMinute_Unlocks) {
        ASSERT_TRUE(listener.ReceivedAchievements.empty());
        gameState->TimeElapsed = OneMinute;
        Unlockables::Tick();

        ASSERT_EQ(listener.ReceivedAchievements.size(), 1);
    }

    TEST(Achievements, Unlockables_WithSaveState_AddsOnlyLockedAchievements) {
        auto save = Achievements::SaveState{};
        save.Achievements0 = 0b0000'0000'0000'0000'0000'0000'0000'1101;
        Achievements::Load(save);

        auto& unlockables = ServiceLocator::Get().GetOrCreate<std::unordered_map<std::string, Unlockable>>();
        ASSERT_FALSE(unlockables.contains(Achievements::Time[0].Name));
        ASSERT_TRUE(unlockables.contains(Achievements::Time[1].Name));
        ASSERT_FALSE(unlockables.contains(Achievements::Time[2].Name));
        ASSERT_FALSE(unlockables.contains(Achievements::Time[3].Name));
    }
} // namespace Invent