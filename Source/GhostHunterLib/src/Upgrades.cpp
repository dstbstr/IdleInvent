#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Tools/Tools.h"
#include "Mechanics/Upgrade.h"

#define UPGRADE(_TypeName)                                                                     \
    namespace _TypeName##_Registry {                                                           \
        using TypeName = _TypeName;                                                            \
        void _register_##_TypeName();                                                          \
        struct _AutoRegister_##_TypeName {                                                     \
            _AutoRegister_##_TypeName() {                                                      \
                _register_##_TypeName();                                                       \
                ::UpgradeManager::_Details::GetInitFns().push_back([] {                        \
                    ::ServiceLocator::Get().CreateIfMissing<PubSub<UpgradeEvent<TypeName>>>(); \
                });                                                                            \
            }                                                                                  \
        } autoRegister_##_TypeName{};                                                          \
    }                                                                                          \
    void _TypeName##_Registry::_register_##_TypeName()

#define P(a, b) \
    std::pair { a, b }
#define ITEM(n, ...)                                                                        \
    ::UpgradeManager::_Details::GetUpgradeCosts<TypeName>()[TypeName::TypeEnum::n] = []() { \
        std::unordered_map<TypeName::LevelEnum, ResourceCollection> result{};               \
        __VA_ARGS__                                                                         \
        return result;                                                                      \
    }();
#define LEVEL(l, ...) result[TypeName::LevelEnum::l] = CreateRc<ResourceType>(__VA_ARGS__);
namespace GhostHunter {
    using LevelName = QualityType;
    using ResourceType = ResourceName;

    // clang-format off
    UPGRADE(Tool) {
        ITEM(Camera,
            LEVEL(Bad, P(ResourceType::Cash, 200ll)) LEVEL(Okay, P(ResourceType::Cash, 400ll))
            LEVEL(PrettyGood, P(ResourceType::Cash, 800ll)) LEVEL(Great, P(ResourceType::Cash, 1'600ll))
            LEVEL(Excellent, P(ResourceType::Cash, 3'200ll)) LEVEL(Wonderful, P(ResourceType::Cash, 6'400ll))
            LEVEL(Legendary, P(ResourceType::Cash, 12'800ll))
        );
        ITEM(Flashlight, 
            LEVEL(Bad, P(ResourceType::Cash, 51ll)));
    }

    UPGRADE(Media) {
        ITEM(Picture,
            LEVEL(Bad, P(ResourceType::Images, 2ll)) LEVEL(Okay, P(ResourceType::Images, 4ll))
            LEVEL(PrettyGood, P(ResourceType::Images, 8ll)) LEVEL(Great, P(ResourceType::Images, 16ll))
            LEVEL(Excellent, P(ResourceType::Images, 32ll)) LEVEL(Wonderful, P(ResourceType::Images, 64ll))
            LEVEL(Legendary, P(ResourceType::Images, 128ll))
        );
        ITEM(Interview,
            LEVEL(Bad, P(ResourceType::Audio, 6ll)) 
            LEVEL(Okay, P(ResourceType::Audio, 12ll))
            LEVEL(PrettyGood, P(ResourceType::Audio, 24ll)) 
            LEVEL(Great, P(ResourceType::Audio, 48ll))
            LEVEL(Excellent, P(ResourceType::Audio, 96ll)) 
            LEVEL(Wonderful, P(ResourceType::Audio, 192ll))
            LEVEL(Legendary, P(ResourceType::Audio, 384ll))
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