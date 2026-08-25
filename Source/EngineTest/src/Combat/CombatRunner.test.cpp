#include "Combat/CombatRunner.h"
#include "TestCombatTypes.h"
#include "CommonTest.h"
#include <memory>

namespace Combat {
	struct CombatRunnerTest : public ::testing::Test {
        using TestRunner = Runner<TestCombatant, TestAction, TestEvent, TestRules>;

        Social::FactionId GoodGuys = Social::ToFactionId(TestFaction::GoodGuys);
        Social::FactionId BadGuys = Social::ToFactionId(TestFaction::BadGuys);

		CombatantId Player{};
        CombatantId Enemy{};

		std::unique_ptr<TestRunner> Runner;
        std::vector<TestEvent> Events;
        std::vector<RunnerStatus> Statuses;
        std::vector<ScopedHandle> Subs;

        void CreateRunner(TestCombatant player = {.Hp = 10, .Attack = 3}, TestCombatant enemy = {.Hp = 5, .Attack = 2}) {
            auto encounter = TestEncounter{TestRules{}, std::make_unique<RoundRobinScheduler>()};
            Player = encounter.AddCombatant(GoodGuys, player);
            Enemy = encounter.AddCombatant(BadGuys, enemy);
            Runner = std::make_unique<TestRunner>(std::move(encounter));

            Subs.push_back(Runner->SubscribeEvents([this](const TestEvent& e) { Events.push_back(e); }));
            Subs.push_back(Runner->SubscribeStatus([this](const RunnerStatus& s) { Statuses.push_back(s); }));
        }

        void SetUp() override { 
            CreateRunner();
        }

        TestController* SetController(CombatantId actor, std::optional<TestAction> action = std::nullopt, bool repeat = false) {
            auto controller = std::make_unique<TestController>();
            controller->NextAction = action;
            controller->Repeat = repeat;

            auto* result = controller.get();
            Runner->SetController(actor, std::move(controller));

            return result;
        }
	};

    TEST_F(CombatRunnerTest, Tick_WithoutController_MissingControllerStatus) { 
        Runner->Tick(OneSecond);
        ASSERT_EQ(1, Statuses.size());
        auto status = Statuses[0];
        ASSERT_EQ(status.State, RunnerState::MissingController);
        ASSERT_EQ(status.Actor, Player);
    }

    TEST_F(CombatRunnerTest, Tick_WithNoInput_WaitingForInputStatus) {
        auto* controller = SetController(Player);
        Runner->Tick(OneSecond);

        ASSERT_EQ(1, Statuses.size());
        auto status = Statuses[0];
        ASSERT_EQ(status.State, RunnerState::WaitingForInput);
        ASSERT_EQ(Player, status.Actor);

        ASSERT_EQ(1, controller->Calls);
    }

    TEST_F(CombatRunnerTest, Tick_WithAttack_DamagesAndEndsTurn) {
        SetController(Player, TestAction{.Target = Enemy});
        Runner->Tick(OneSecond);

        ASSERT_EQ(1, Statuses.size());
        auto status = Statuses[0];
        ASSERT_EQ(status.State, RunnerState::MissingController); // enemy has no controller
        
        ASSERT_EQ(2, Events.size());
        ASSERT_TRUE(Events[0].Kind == TestEventKind::Damaged || Events[1].Kind == TestEventKind::Damaged);
        ASSERT_TRUE(Events[0].Kind == TestEventKind::TurnEnded || Events[1].Kind == TestEventKind::TurnEnded);
    }

    TEST_F(CombatRunnerTest, Tick_WithInvalidAction_InvalidActionStatus) {
        SetController(Player, TestAction{.Target = {42}});
        Runner->Tick(OneSecond);

        ASSERT_EQ(1, Statuses.size());
        auto status = Statuses[0];
        ASSERT_TRUE(status.Error.has_value());
        ASSERT_EQ(*status.Error, SubmitError::InvalidAction);
        ASSERT_EQ(status.State, RunnerState::Error);
        ASSERT_EQ(Player, status.Actor);

        ASSERT_TRUE(Events.empty());
    }

    TEST_F(CombatRunnerTest, Tick_WithDefeat_EndsEncounter) {
        SetController(Player, TestAction{.Target = Enemy}, true);
        SetController(Enemy, TestAction{.Target = Player}, true);
        Runner->Tick(OneSecond); // runs up to 16 steps
        ASSERT_EQ(1, Statuses.size());
        auto status = Statuses[0];
        ASSERT_EQ(status.State, RunnerState::Finished);

        auto defeated = std::find_if(Events.begin(), Events.end(), [](const auto& e) {
            return e.Kind == TestEventKind::Defeated;
        });
        auto ended = std::find_if(Events.begin(), Events.end(), [](const auto& e) {
            return e.Kind == TestEventKind::CombatEnded;
        });
        ASSERT_NE(defeated, Events.end());
        ASSERT_NE(ended, Events.end());
    }

    TEST_F(CombatRunnerTest, Tick_WithInactiveCombatant_Skips) {
        CreateRunner({.Hp = 0, .Attack = 3});
        auto* enemyController = SetController(Enemy);
        Runner->Tick(OneSecond);

        auto skipped = std::ranges::find_if(Events, [](const auto& e) { return e.Kind == TestEventKind::TurnSkipped; });

        ASSERT_NE(skipped, Events.end());
        ASSERT_EQ(skipped->Actor, Player);

        ASSERT_EQ(1, enemyController->Calls);
        auto status = Runner->GetStatus();
        ASSERT_EQ(RunnerState::WaitingForInput, status.State);
        ASSERT_EQ(Enemy, status.Actor);
    }
}