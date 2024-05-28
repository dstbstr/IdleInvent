#include "CommonTest.h"
#include "InventLib/Resources/Storage.h"
#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

namespace Invent {
    struct StorageTest : public ::testing::Test {
		void SetUp() override {
			ServiceLocator::Get().Set<GameState>();
		}

		void TearDown() override {
            ServiceLocator::Get().ResetAll();
        }
	
		Advancement DefaultAdv{"Test", AdvancementCosts::Linear<10>};
        ResourceCollection ManyResources = []() {
            ResourceCollection resources;
			for (auto& [name, resource] : resources) {
				resource.Current = 100;
			}
            return resources;
        }();
        ResourceCollection NoResources{};
	};

    TEST_F(StorageTest, CanLevelUp_WithoutResources_ReturnsFalse) {
		Storage storage{ResourceName::Labor, DefaultAdv};
        storage.ResourcesToLevel = ManyResources;

		auto& gameState = ServiceLocator::Get().GetRequired<GameState>();
        gameState.CurrentResources = NoResources;

		ASSERT_FALSE(storage.CanLevelUp());
	}
} // namespace Invent