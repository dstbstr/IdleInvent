#include "GhostHunter/Media/Media.h"

#include "GhostHunter/Resources/GhostHunterResources.h"
#include "Mechanics/Purchasable.h"

namespace {
    ResourceCollection GetBaseCost(GhostHunter::MediaType type) {
        using namespace GhostHunter;
        (void)type;
        return CreateRc<ResourceName>(std::pair(ResourceName::Physical, 3));    
    }
    f32 MultiplierByQuality(GhostHunter::QualityType quality) {
        switch(quality) {
            using enum GhostHunter::QualityType;
            case Awful: return 2.0f;
            case Bad: return 3.0f;
            case Okay: return 4.0f;
            case PrettyGood: return 5.0f;
            case Great: return 6.0f;
            case Excellent: return 7.0f;
            case Wonderful: return 8.0f;
            default: return 1.0f;
        }
    }
}
namespace GhostHunter {
    // TODO: Probably better to represent this as another purchasable
    ResourceCollection UpgradeCost(const Media& media) {
        auto baseCost = GetBaseCost(media.Type);
        auto multiplier = MultiplierByQuality(media.Quality);
        return baseCost * multiplier;
    }

    void Media::Initialize() {
        ServiceLocator::Get().CreateIfMissing<PubSub<Purchase<MediaType>>>();
        for(auto type : Enum::GetAllValues<MediaType>()) {
            Purchasables::Add(type, GetBaseCost(type));
        }
    }

    void Media::ShutDown() {
        //nothing for now
    }
}