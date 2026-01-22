#include "CommonTest.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Mechanics/Sale.h"

namespace GhostHunter {

    struct MarketTest : public ::testing::Test {
        PubSub<Sale<Media>>* pubSub {nullptr};
        ResourceCollection resources{CreateRc<ResourceName>()};
        Market market{&resources};

        static void SetUpTestSuite() { Market::Initialize(); }
        static void TearDownTestSuite() { ServiceLocator::Get().ResetAll(); }
        void SetUp() override {
            pubSub = ServiceLocator::Get().Get<PubSub<Sale<Media>>>();
        }

        void Sell(u32 value) {
            Media media{MediaType::Book, value};
            pubSub->Publish({media});
        }
    };

    TEST_F(MarketTest, MediaSoldIsAddedToMarket) {
        Sell(100);
        market.Update(OneSecond);
        ASSERT_EQ(resources.at(ResourceName::Cash).Current, 100);
    }

    TEST_F(MarketTest, MediaSold_AfterTwoSeconds_IsDecayed) {
        Sell(100);
        market.Update(OneSecond * 2);
        ASSERT_LT(resources.at(ResourceName::Cash).Current, 200);
    }

    TEST_F(MarketTest, MediaSold_After8Hours_RunsQuickly) { 
        Sell(10'000'000);
        market.Update(OneHour * 8);
        ASSERT_GT(resources.at(ResourceName::Cash).Current, 100);
    }
    TEST_F(MarketTest, MediaSold_WithOverflowValue_ClampsAtMax) { 
        Sell(std::numeric_limits<u32>::max());
        market.Update(OneDay);
        ASSERT_EQ(std::numeric_limits<u32>::max(), resources.at(ResourceName::Cash).Current);
    }
}