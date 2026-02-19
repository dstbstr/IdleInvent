#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Tools/Tools.h"

#include "Mechanics/Purchasable.h"

#define PURCHASE(_Type)                                                                 \
    namespace _Type##_Registry {                                                        \
        using TypeName = _Type;                                                         \
        void _register_##_Type();                                                       \
        struct _AutoRegister_##_Type {                                                  \
            _AutoRegister_##_Type() {                                                   \
                _register_##_Type();                                                    \
                ::_PurchaseDetails::GetInitFns().push_back([] {                         \
                    ::ServiceLocator::Get().CreateIfMissing<PubSub<Purchase<_Type>>>(); \
                });                                                                     \
            }                                                                           \
        } autoRegister_##_Type{};                                                       \
    }                                                                                   \
    void _Type##_Registry::_register_##_Type()

#define P(r, a) \
    std::pair { ResourceType::r, a }
#define ITEM(n, ...) Purchasables::Add(TypeName::n, CreateRc<ResourceName>(__VA_ARGS__));
namespace GhostHunter {
    using ResourceType = ResourceName;
    // clang-format off
    PURCHASE(ToolName) {
        ITEM(Flashlight, P(Cash, 100));
        ITEM(Camera, P(Cash, 250), P(Images, 10));
        ITEM(EmfDetector, P(Cash, 350));
        ITEM(ThermalCamera, P(Cash, 1'000));
        ITEM(EvpRecorder, P(Cash, 10'000));
        ITEM(LaserGrid, P(Cash, 100'000));
        ITEM(SpiritBox, P(Cash, 1'000'000));
    }

    // Cash, Images, Audio, Video, Physical

    PURCHASE(MediaType) {
        ITEM(Picture, P(Physical, 1));
        ITEM(Interview, P(Audio, 3));
        ITEM(Article, P(Images, 3), P(Physical, 2));
        ITEM(Book, P(Physical, 10), P(Images, 20));
        ITEM(Podcast, P(Audio, 10), P(Physical, 35));
        ITEM(TvShow, P(Audio, 50), P(Video, 100), P(Physical, 25));
        ITEM(Movie, P(Video, 250), P(Cash, 100'000));
    }

    PURCHASE(LocationName) {
        ITEM(Shed, P(Cash, 10));
        ITEM(School, P(Cash, 200));
        ITEM(House, P(Cash, 4'000));
        ITEM(Church, P(Cash, 50'000));
        ITEM(Hotel, P(Cash, 100'000));
        ITEM(Prison, P(Cash, 500'000));
        ITEM(Hospital, P(Cash, 750'000));
        ITEM(Sanitorium, P(Cash, 1'000'000));
    }
    // clang-format on
} // namespace GhostHunter

#undef PURCHASE
#undef ITEM
#undef P

// need a method to prevent dropping these symbols
void InitializePurchases() { Purchasables::InitializeAll(); }