#include "CommonTest.h"
#include "TestGameState.h"

#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/GameState/GameState.h"

namespace Invent {
    struct TestPurchasable : public Purchasable {
        TestPurchasable(ResourceCollection costs, Purchasable::OnPurchaseFn onPurchase)
            : Purchasable("TestPurchasable", "Test Description", "Cost Description", costs, onPurchase) {}
    };

    struct PurchasableTest : public testing::Test {
        void SetUp() override {
            GenerateTestGameState();
            auto& services = ServiceLocator::Get();
            purchasables = &services.GetOrCreate<std::unordered_map<std::string, Purchasable>>();
            gameState = &services.GetOrCreate<GameState>();
            services.CreateIfMissing<PubSub<FileOperation>>();
        }

        void TearDown() override {
            ServiceLocator::Get().ResetAll();
        }

        void RegisterPurchasable(TestPurchasable p) {
            purchasables->emplace(p.Name, p);
        }

        bool IsRegistered(TestPurchasable p) {
            return purchasables->contains(p.Name);
        }

        ResourceCollection LargeCollection = []() {
            ResourceCollection collection;
            for(auto resource: AllResources) {
                collection[resource].Current = 1000;
            }
            return collection;
        }();

        ResourceCollection EmptyCollection{};

        std::unordered_map<std::string, Purchasable>* purchasables{nullptr};
        GameState* gameState{nullptr};
    };

    TEST_F(PurchasableTest, CanPurchase_MissingResources_ReturnsFalse) {
        auto p = TestPurchasable{LargeCollection, []() {}};

        ASSERT_FALSE(p.CanPurchase(EmptyCollection));
    }

    TEST_F(PurchasableTest, CanPurchase_WithResources_ReturnsTrue) {
        auto p = TestPurchasable{EmptyCollection, []() {}};

        ASSERT_TRUE(p.CanPurchase(LargeCollection));
    }

    TEST_F(PurchasableTest, Purchase_WithoutResources_TriggersOnPurchase) {
        bool purchased = false;
        auto p = TestPurchasable{EmptyCollection, [&purchased]() {purchased = true;}};
        RegisterPurchasable(p);

        p.Purchase(EmptyCollection);

        ASSERT_TRUE(purchased);
    }

    TEST_F(PurchasableTest, Purchase_WithoutResources_SetsValuesNegative) {
        auto p = TestPurchasable{LargeCollection, []() {}};
        RegisterPurchasable(p);

        p.Purchase(EmptyCollection);
        for (auto resource : SecondaryResources) {
            ASSERT_EQ(EmptyCollection[resource].Current, -1000);
        }
    }

    TEST_F(PurchasableTest, Purchasables_AfterPurchase_DoesNotContainPurchasable) {
        auto p = TestPurchasable{EmptyCollection, []() {}};
        RegisterPurchasable(p);
        ASSERT_TRUE(IsRegistered(p));
        p.Purchase(LargeCollection);

        ASSERT_FALSE(IsRegistered(p));
    }

    TEST_F(PurchasableTest, TryPurchase_WithoutResources_DoesNotTriggerPurchase) {
        ServiceLocator::Get().CreateIfMissing<GameState>();
        bool purchased = false;
        auto p = TestPurchasable{LargeCollection, [&purchased]() {purchased = true;}};
        RegisterPurchasable(p);

        Purchasables::TryPurchase(p.Name);
        Purchasables::Tick();

        ASSERT_FALSE(purchased);
    }

    TEST_F(PurchasableTest, TryPurchase_WithResources_TriggersPurchase) {
        bool purchased = false;
        auto p = TestPurchasable{EmptyCollection, [&purchased]() {purchased = true;}};
        RegisterPurchasable(p);

        Purchasables::TryPurchase(p.Name);
        Purchasables::Tick();

        ASSERT_TRUE(purchased);
    }
} // namespace Invent