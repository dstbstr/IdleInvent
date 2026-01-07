#include "CommonTest.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

namespace GhostHunter {

    struct MarketTest : public ::testing::Test {
        PubSub<Media>* pubSub;
        GhostHunterResources resources;

        static void TearDownTestSuite() { ServiceLocator::Get().ResetAll(); }
        void SetUp() override {
            auto& services = ServiceLocator::Get();
            pubSub = &services.GetOrCreate<PubSub<Media>>();
            services.Set<GhostHunterResources>(resources);

            Market::Init(); 
        }
    };

    TEST_F(MarketTest, MediaSoldIsAddedToMarket) {
        Media media{MediaType::Book, 100};
        pubSub->Publish(media);
        Market::Update(OneSecond);
        ASSERT_EQ(resources.GetResource(ResourceName::Cash), 100);
    }
}