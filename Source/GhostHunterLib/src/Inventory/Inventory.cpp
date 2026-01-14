#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Quality/Quality.h"
#include "Mechanics/Purchasable.h"
#include "Mechanics/Sale.h"
#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

namespace {
    size_t toolPurchaseHandle{0};
}

namespace GhostHunter::Inventory {
	std::vector<Media>& GetMedia() {
		static std::vector<Media> media;
		return media;
    }

    void SellAllMedia() { 
        auto& media = GetMedia();
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<Media>>>();
        for(const auto& e : media) {
            pubSub.Publish({e});
        }
        media.clear();
    }

    void SellMedia(size_t index) {
        auto& media = GetMedia();
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<Media>>>();
        if(index < media.size()) {
            pubSub.Publish({media[index]});
            media.erase(media.begin() + index);
        }
    }

    std::vector<Tool>& GetTools() {
        static std::vector<Tool> tools;
        return tools;
    }

    void Initialize() {
        auto& services = ServiceLocator::Get();
        toolPurchaseHandle = services.GetRequired<PubSub<Purchase<ToolName>>>().Subscribe([](const Purchase<ToolName>& purchase) {
            GetTools().push_back(Tool(purchase.Id, QualityType::Awful));
        });
    }

    void ShutDown() {
        GetMedia().clear();
        GetTools().clear();
        auto& services = ServiceLocator::Get();
        services.GetRequired<PubSub<Purchase<ToolName>>>().Unsubscribe(toolPurchaseHandle);
    }
} // namespace GhostHunter::Inventory