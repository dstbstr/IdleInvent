#include "CommonTest.h"
#include "TestGameState.h"

#include "InventLib/Technology/Technology.h"
#include "Resources/Storage.h"

/*
namespace Invent {
    TEST(StorageTest, CanUnlock_With1000Resources_ReturnsTrue) {
        auto unlockable = Technologies::StorageTechs[0].MakeUnlockable(0);
        auto& resources = GenerateTestGameState().Character.CurrentResources;
        for(const auto& name: AllResources) {
            resources.at(name).Current = 1'000;
        }

        ASSERT_TRUE(unlockable.UnlockCondition());
    }

    TEST(StorageTest, Load_WithDefaultSave_HasMaxGreaterThanZero) {
        StorageSave save{};
        StorageCollection collection{};
        collection.Load(save);
        for(const auto& storage: collection) {
            ASSERT_GT(storage.second.Capacity, 0);
        }
    }

    TEST(StorageTest, Load_WithValidSave_AppliesStoragesInOrder) {
        StorageSave save{
            .Stored = {5, 10, 15, 20, 25},
            .Capacity = 0b0'111'110'101'100'011
        };

        StorageCollection collection{};
        collection.Load(save);
        for(auto i = 1u; i < AllResources.size(); i++) {
            const auto& current = collection.at(AllResources[i]);
            const auto& previous = collection.at(AllResources[i - 1]);
            ASSERT_TRUE(previous.Stored < current.Stored);
            ASSERT_TRUE(previous.Capacity < current.Capacity);
		}   
    }

    TEST(StorageTest, Save_WithResources_StoresResourcesInOrder) { 
        StorageCollection storage{};
        auto value = 1;
        for(auto name : AllResources) {
            auto& current = storage.at(name);
            current.Stored = value * 10;
            current.UpgradeCapacity(value);
            value++;
		}

        StorageSave save{};
		storage.Save(save);
		for(auto i = 1u; i < AllResources.size(); i++) {
            const auto& current = storage.at(AllResources[i]);
            const auto& previous = storage.at(AllResources[i - 1]);
            ASSERT_TRUE(previous.Stored < current.Stored);
            ASSERT_TRUE(previous.Capacity < current.Capacity);
		}
    }

    TEST(StorageTest, Load_SavedState_MatchesOriginal) {
        StorageCollection original{};
        auto value = 1;
        for(auto name: AllResources) {
            original.at(name).Stored = value * 10;
            original.at(name).UpgradeCapacity(value);
            value++;
        }

        StorageSave save{};
        original.Save(save);
        StorageCollection loaded{};
        loaded.Load(save);

        for(auto name: AllResources) {
            const auto lhs = original.at(name);
            const auto rhs = loaded.at(name);
            
            ASSERT_NEAR(static_cast<double>(lhs.Stored), static_cast<double>(rhs.Stored), lhs.Stored * 0.2);
            ASSERT_EQ(lhs.Capacity, rhs.Capacity);
        }
    }
} // namespace Invent
*/