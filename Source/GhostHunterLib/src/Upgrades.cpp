#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Tools/Tools.h"
#include "Mechanics/Upgrade.h"

#define UPGRADE(_TypeName)                                                                     \
    namespace _TypeName##_UpgradeRegistry {                                                           \
        using TypeName = _TypeName;                                                            \
        void _register();                                                                      \
        struct _AutoRegister {                                                                 \
            _AutoRegister() {                                                                  \
                _register();                                                                   \
                ::UpgradeManager::_Details::GetInitFns().push_back([] {                        \
                    ::ServiceLocator::Get().CreateIfMissing<PubSub<UpgradeEvent<TypeName>>>(); \
                });                                                                            \
            }                                                                                  \
        } autoRegister{};                                                                      \
    }                                                                                          \
    void _TypeName##_UpgradeRegistry::_register()

#define P(_resource, b) \
    std::pair { ResourceType::_resource, b }
#define ITEM(n, ...)                                                                      \
    ::UpgradeManager::_Details::GetUpgradeCosts<TypeName>()[TypeName::IdType::n] = []() { \
        std::unordered_map<TypeName::LevelType, ResourceCollection> result{};             \
        __VA_ARGS__                                                                       \
        return result;                                                                    \
    }();
#define LEVEL(l, ...) result[TypeName::LevelType::l] = CreateRc<ResourceType>(__VA_ARGS__);
namespace GhostHunter {
    using LevelName = QualityType;
    using ResourceType = ResourceName;

    // clang-format off
    UPGRADE(Tool) {
        ITEM(Camera,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(Flashlight, 
            LEVEL(Bad, P(Cash, 51ll))
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(EmfDetector,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(ThermalCamera,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(EvpRecorder,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(LaserGrid,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
        ITEM(SpiritBox,
            LEVEL(Bad, P(Cash, 200ll)) 
            LEVEL(Okay, P(Cash, 400ll))
            LEVEL(PrettyGood, P(Cash, 800ll)) 
            LEVEL(Great, P(Cash, 1'600ll))
            LEVEL(Excellent, P(Cash, 3'200ll)) 
            LEVEL(Wonderful, P(Cash, 6'400ll))
            LEVEL(Legendary, P(Cash, 12'800ll))
        );
    }

    UPGRADE(Media) {
        ITEM(Picture,
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
        ITEM(Interview,
            LEVEL(Bad, P(Audio, 6ll)) 
            LEVEL(Okay, P(Audio, 12ll))
            LEVEL(PrettyGood, P(Audio, 24ll)) 
            LEVEL(Great, P(Audio, 48ll))
            LEVEL(Excellent, P(Audio, 96ll)) 
            LEVEL(Wonderful, P(Audio, 192ll))
            LEVEL(Legendary, P(Audio, 384ll))
        );
        ITEM(Article,             
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
        ITEM(Book,             
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
        ITEM(Podcast,             
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
        ITEM(TvShow,             
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
        ITEM(Movie,             
            LEVEL(Bad, P(Images, 2ll)) 
            LEVEL(Okay, P(Images, 4ll))
            LEVEL(PrettyGood, P(Images, 8ll)) 
            LEVEL(Great, P(Images, 16ll))
            LEVEL(Excellent, P(Images, 32ll)) 
            LEVEL(Wonderful, P(Images, 64ll))
            LEVEL(Legendary, P(Images, 128ll))
        );
    }
    // clang-format on
} // namespace GhostHunter

#undef LEVEL
#undef P
#undef UPGRADE

void InitializeUpgrades() {
    for(const auto& fn: UpgradeManager::_Details::GetInitFns()) {
        fn();
    }
}