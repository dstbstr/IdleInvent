#include "Mechanics/Sale.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Resources/GhostHunterResources.h"

#define SALE(_TypeName)                                                                \
    namespace _TypeName##_Registry {                                                   \
        using TypeName = _TypeName;                                                    \
        void _register();                                                              \
        struct _AutoRegister {                                                         \
            _AutoRegister() {                                                          \
                _register();                                                           \
                ::SalesManager::_Details::GetInitFns().push_back([] {                  \
                    ::ServiceLocator::Get().CreateIfMissing<PubSub<Sale<TypeName>>>(); \
                });                                                                    \
            }                                                                          \
        } autoRegister{};                                                              \
    }                                                                                  \
    void _TypeName##_Registry::_register()

#define ITEM(n, ...)                                                                  \
    ::SalesManager::_Details::GetSaleValues<TypeName>()[TypeName::IdType::n] = []() { \
        std::unordered_map<TypeName::LevelType, ResourceCollection> result{};         \
        __VA_ARGS__                                                                   \
        return result;                                                                \
    }();
#define LEVEL(l, value) result[TypeName::LevelType::l] = CreateRc<ResourceType>({ResourceName::Cash, value});

// clang-format off
     SALE(Media) {
        ITEM(Picture,
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(Interview,
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(Article,             
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(Book,             
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(Podcast,             
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(TvShow,             
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
        ITEM(Movie,             
            LEVEL(Bad, 2) 
            LEVEL(Okay, 4)
            LEVEL(PrettyGood, 6) 
            LEVEL(Great, 8)
            LEVEL(Excellent, 10) 
            LEVEL(Wonderful, 12)
            LEVEL(Legendary, 14)
        );
    }

// clang-format on
#undef LEVEL
#undef P
#undef SALE

void InitializeSales() {
    for(const auto& fn : ::SalesManager::_Details::GetInitFns()) {
        fn();
    }
}