#include "CommonTest.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

namespace GhostHunter {

    struct MarketTest : public ::testing::Test {
        PubSub<Media>* pubSub {nullptr};
        GhostHunterResources* resources{nullptr};

        static void TearDownTestSuite() { ServiceLocator::Get().ResetAll(); }
        void SetUp() override {
            auto& services = ServiceLocator::Get();
            pubSub = &services.GetOrCreate<PubSub<Media>>();
            resources = &services.GetOrCreate<GhostHunterResources>();
        }
        void TearDown() override {
            resources->Clear();
        }
    };

    TEST_F(MarketTest, MediaSoldIsAddedToMarket) {
        Market market;
        Media media{MediaType::Book, 100};
        pubSub->Publish(media);
        market.Update(OneSecond);
        ASSERT_EQ(resources->GetResource(ResourceName::Cash), 100);
    }

    TEST_F(MarketTest, MediaSold_AfterTwoSeconds_IsDecayed) {
        Market market;
        Media media{MediaType::Book, 100};
        pubSub->Publish(media);
        market.Update(OneSecond * 2);
        ASSERT_LT(resources->GetResource(ResourceName::Cash), 200);
    }

    TEST_F(MarketTest, MediaSold_After8Hours_RunsQuickly) { 
        Market market;
        Media media{MediaType::Book, 10'000'000};
        pubSub->Publish(media);
        market.Update(OneHour * 8);
        ASSERT_GT(resources->GetResource(ResourceName::Cash), 100);
    }
    TEST_F(MarketTest, MediaSold_WithOverflowValue_ClampsAtMax) { 
        Market market;
        Media media{MediaType::Book, std::numeric_limits<u32>::max()};
        pubSub->Publish(media);
        market.Update(OneDay);
        ASSERT_EQ(std::numeric_limits<u32>::max(), resources->GetResource(ResourceName::Cash));
    }
}