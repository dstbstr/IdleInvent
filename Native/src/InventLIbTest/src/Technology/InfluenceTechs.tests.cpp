#include "CommonTest.h"
#include "TestGameState.h"

#include "InventLib/Technology/Invention.h"
#include "InventLib/Technology/Technology.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Technology/TechAge.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include <vector>

/*
namespace Invent {

    struct AgeListener {
		void OnAgeChange(const TechAge& age) {
			ages.push_back(age);
		}

		std::vector<TechAge> ages{};
	};

    struct InfluenceTechTest : public ::testing::Test {
        GameState* gameState{nullptr};
        std::unordered_map<std::string, Purchasable>* purchasables{nullptr};
        AgeListener ageListener;

        void SetUp() override {
            auto& services = ServiceLocator::Get();
            GenerateTestGameState();
            gameState = &services.GetOrCreate<GameState>();
            purchasables = &services.GetOrCreate<std::unordered_map<std::string, Purchasable>>();
            services.CreateIfMissing<std::unordered_map<std::string, Unlockable>>();
            services.CreateIfMissing<PubSub<InventionLevel>>();
            services.GetOrCreate<PubSub<TechAge>>().Subscribe([this](const TechAge& age) {
                ageListener.OnAgeChange(age);
            });
        }

        void TearDown() override {
            ServiceLocator::Get().ResetAll();
		}

        void AddResources() {
            for (auto& [name, resource] : gameState->Character.CurrentResources) {
				resource.Current = std::numeric_limits<s64>::max();
			}
		}
    };

    TEST_F(InfluenceTechTest, MakeUnlockable_WithInvention_CreatesFirstUnlockable) {
        auto firstTech = Technologies::InfluenceTechs[0];
        auto unlockable = firstTech.MakeUnlockable(0);
        ASSERT_THAT(unlockable.Name, StartsWith(firstTech.Name));
    }

    TEST_F(InfluenceTechTest, Purchase_FirstInvention_PublishesAgeChange) {
        auto p = Technologies::InfluenceTechs[0].MakePurchasable(0);
        ResourceCollection resources{};
        p.Purchase(resources);

        ASSERT_EQ(ageListener.ages.size(), 1);
    }

    TEST_F(InfluenceTechTest, Unlockables_AfterInitialization_CanAllBeUnlocked) {
        auto& unlockables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Unlockable>>();
        unlockables.clear();
        for(const auto& tech: Technologies::InfluenceTechs) {
            auto unlockable = tech.MakeUnlockable(0);
			unlockables.emplace(unlockable.Name, unlockable);
        }

        AddResources();

        while(!unlockables.empty()) {
            Unlockables::Tick();
            if(!purchasables->empty()) {
                const auto& name = purchasables->begin()->first;
            	Purchasables::TryPurchase(name);
                Purchasables::Tick();
            }
        }
    }

    TEST_F(InfluenceTechTest, Purchasables_MostExpensive_IsLessThanOneHundredMillion) {
        auto p = Technologies::InfluenceTechs | std::views::transform([](const Invention& tech) { return tech.MakePurchasable(2); });

        ResourceCollection maxResources{};
        for (auto& [name, resource] : maxResources) {
            resource.Current = 100'000'000;
		}

        for(const auto& purchasable: p) {
            ASSERT_TRUE(purchasable.CanPurchase(maxResources)) << purchasable.Name;
        }
    }
} // namespace Invent
*/