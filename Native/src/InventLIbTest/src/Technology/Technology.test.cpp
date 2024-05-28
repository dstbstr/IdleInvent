#include "CommonTest.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include "InventLib/Technology/Technology.h"
#include "InventLib/Technology/Invention.h"
#include "InventLib/GameState/GameState.h"
#include "Core/Utilities/IRandom.h"

namespace Invent {
	struct TechnologyListener {
		TechnologyListener() {
			handle = ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>().Subscribe([this](const InventionLevel& invention) {
				ReceivedInventions.push_back(invention);
				});
		}
		~TechnologyListener() {
			ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>().Unsubscribe(handle);
		}

		std::vector<InventionLevel> ReceivedInventions;
		size_t handle{ 0 };
	};

	struct TechnologyTest : public ::testing::Test {
		void SetUp() override {
			auto& services = ServiceLocator::Get();

			services.SetThisAsThat<DefaultRandom, IRandom>();
			Technologies::Initialize();
			purchasables = &services.GetOrCreate<std::unordered_map<std::string, Purchasable>>();
			unlockables = &services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
			gameState = &services.GetOrCreate<GameState>();
		}

		void TearDown() override {
			ServiceLocator::Get().ResetAll();
		}

		std::unordered_map<std::string, Unlockable>* unlockables;
		std::unordered_map<std::string, Purchasable>* purchasables;
		GameState* gameState{ nullptr };
	};

	TEST_F(TechnologyTest, Unlockables_AfterInitialization_ContainsStoneAgeTechs) {
		ASSERT_EQ(unlockables->size(), Technologies::StoneAgeTechs.size());
	}

	TEST_F(TechnologyTest, Purchasables_AfterInitialization_ContainsNoElements) {
		ASSERT_TRUE(purchasables->empty());
	}

	TEST_F(TechnologyTest, Unlockables_AfterUnlockingAnInvention_StillContainsOtherUnlockables) {
		gameState->CurrentResources[ResourceName::Influence].Current = 1000;
		gameState->CurrentResources[ResourceName::Wealth].Current = 1;

		Unlockables::Tick();

		ASSERT_EQ(unlockables->size(), Technologies::StoneAgeTechs.size()); // Removed Tribes 1, added Tribes 2
	}

	TEST_F(TechnologyTest, Purchasables_AfterUnlockingAnInvention_ContainsOneElement) {
		gameState->CurrentResources[ResourceName::Influence].Current = 1000;
		gameState->CurrentResources[ResourceName::Wealth].Current = 1;

		Unlockables::Tick();

		ASSERT_EQ(purchasables->size(), 1);
	}

	TEST_F(TechnologyTest, Unlockables_AfterPurchasingStoneAgeTech_RemovesOtherStoneAgeTechs) {
		gameState->CurrentResources[ResourceName::Influence].Current = 1000;
		gameState->CurrentResources[ResourceName::Wealth].Current = 2;
		gameState->CurrentResources[ResourceName::Knowledge].Current = 1;

		std::vector<std::string> unlockableNames = *unlockables | std::views::keys | std::ranges::to<std::vector<std::string>>();
		Unlockables::Tick();

		Purchasables::TryPurchase(purchasables->begin()->first);
		Purchasables::Tick();

		for (const auto& name : unlockableNames) {
			ASSERT_FALSE(unlockables->contains(name));
		}
	}
}