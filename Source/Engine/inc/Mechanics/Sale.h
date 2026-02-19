#pragma once

#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"
#include "Utilities/EnumUtils.h"
#include "Resources/Resource.h"

// Simple Wrapper around a sale
//template<typename T>
//struct Sale {
//    T Item;
//};

template<typename T>
concept SellableType = DescribeSelf<T> && HasId<T> && 
    requires { typename T::LevelType; } &&
    LevelType<typename T::LevelType> && 
    requires(T t) {
        { t.Level } -> std::convertible_to<typename T::LevelType>;
    };

template<SellableType T>
struct Sale {
    T::IdType Id;
    T::LevelType Level;
};

template<SellableType T>
void SellAll(std::vector<T>& items) {
    auto& ps = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
    for(const auto& e: items) {
        ps.Publish({e.Id, e.Level});
    }

    items.clear();
}

template<SellableType T>
void Sell(std::vector<T>& items, size_t index) {
    if(index < items.size()) {
        auto& item = items[index];
        auto& ps = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
        ps.Publish({item.Id, item.Level});
        items.erase(items.begin() + index);
    }
}

namespace SalesManager {
    namespace _Details {
        template<SellableType T>
        std::unordered_map<typename T::IdType, std::unordered_map<typename T::LevelType, ResourceCollection>>&
        GetSaleValues() {
            static std::unordered_map<typename T::IdType, std::unordered_map<typename T::LevelType, ResourceCollection>>
                values;
            return values;
        }
        template<SellableType T>
        std::unordered_map<typename T::IdType, std::function<ResourceCollection(typename T::LevelType)>>&
        GetSaleValueFns() {
            static std::unordered_map<typename T::IdType, std::function<ResourceCollection(typename T::LevelType)>>
                valueFns;
            return valueFns;
        }
    }

    template<SellableType T>
    std::optional<ResourceCollection> TryGetValue(typename T::IdType id, typename T::LevelType level) {
        auto nextLevel = level;
        if constexpr(CountEnum<typename T::LevelType>) {
            nextLevel = Enum::Increment(level);
        } else {
            ++nextLevel;
        }
        if(nextLevel == level) return {};

        const auto& valueFns = _Details::GetSaleValueFns<T>();
        if(valueFns.contains(id)) {
            return valueFns.at(id)(nextLevel);
        }

        const auto& idValues = _Details::GetSaleValues<T>();
        if(!idValues.contains(id)) return {};
        const auto& levelValues = idValues.at(id);
        if(!levelValues.contains(nextLevel)) {
            return {};
        }

        return levelValues.at(nextLevel);
    }
    namespace _Details {
        inline std::vector<std::function<void()>>& GetInitFns() {
            static std::vector<std::function<void()>> fns{};
            return fns;
        }
    } // namespace SalesManager::_Details
} // namespace SalesManager

/*
template<typename T>
void SellAll(std::vector<T>& items) {
    auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
    for(const auto& e : items) {
        pubSub.Publish({e});
    }
    items.clear();
}

template<typename T>
void Sell(std::vector<T>& items, size_t index) {
    if(index < items.size()) {
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<Sale<T>>>();
        pubSub.Publish({items[index]});
        items.erase(items.begin() + index);
    }
}
*/