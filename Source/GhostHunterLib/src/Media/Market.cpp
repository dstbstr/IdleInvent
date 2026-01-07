#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Media/Media.h"

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

namespace {
    size_t mediaHandle = 0;
	void OnMediaSold(const GhostHunter::Media& media) {
		// Handle media sold event
    }
}
namespace GhostHunter::Market {
	void Init() {
		auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
        mediaHandle = pubSub.Subscribe(OnMediaSold);
	}

	void ShutDown() {
		auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Media>>();
		pubSub.Unsubscribe(mediaHandle);
    }

	void Update(BaseTime elapsed) {

	}
}