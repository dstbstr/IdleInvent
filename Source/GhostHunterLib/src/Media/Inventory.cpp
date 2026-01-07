#include "GhostHunter/Media/Inventory.h"

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"

namespace GhostHunter::Inventory {
	std::vector<Media>& GetMedia() {
		static std::vector<Media> media;
		return media;
    }

    void SellAllMedia() { 
        auto& media = GetMedia();
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
        for(const auto& e : media) {
            pubSub.Publish(e);
        }
        media.clear();
    }

    void SellMedia(size_t index) {
        auto& media = GetMedia();
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
        if(index < media.size()) {
            pubSub.Publish(media[index]);
            media.erase(media.begin() + index);
        }
    }

    std::vector<Tool>& GetTools() {
        static std::vector<Tool> tools;
        return tools;
    } 
} // namespace GhostHunter::Inventory