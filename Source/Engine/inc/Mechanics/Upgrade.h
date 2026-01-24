#pragma once

#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Instrumentation/Logging.h"
#include "Resources/Resource.h"
#include "Utilities/EnumUtils.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <concepts>
#include <iterator>

template<typename T>
concept LevelType = CountEnum<T> || (std::incrementable<T> && std::equality_comparable<T>);

template<typename T>
concept UpgradableType = DescribeSelf<T> && HasId<T> && requires(T t) {
    typename T::LevelType;
    { t.OnUpgrade() };
    { t.Level } -> std::convertible_to<typename T::LevelType>;
};

template<UpgradableType T>
struct UpgradeEvent {
    const T& Upgrade;
    std::string Description;
};

namespace UpgradeManager {
    namespace _Details {
        template<UpgradableType T>
        std::unordered_map<typename T::IdType, std::unordered_map<typename T::LevelType, ResourceCollection>>&
        GetUpgradeCosts() {
            static std::unordered_map<typename T::IdType, std::unordered_map<typename T::LevelType, ResourceCollection>> costs;
            return costs;
        }
        template<UpgradableType T>
        std::unordered_map<typename T::IdType, std::function<ResourceCollection(typename T::LevelType)>>& GetUpgradeCostFns() {
            static std::unordered_map<typename T::IdType, std::function<ResourceCollection(typename T::LevelType)>> costFns;
            return costFns;
        }
    } // namespace _Details

    template<UpgradableType T>
    std::optional<ResourceCollection> TryGetCost(const T& t) {
        auto level = t.Level;
        if constexpr(CountEnum<typename T::LevelType>) {
            level = Enum::Increment(level);
        } else {
            ++level;
        }
        if(level == t.Level) return {};

        const auto& costFns = _Details::GetUpgradeCostFns<T>();
        if(costFns.contains(t.Id)) {
            return costFns.at(t.Id)(level);
        }

        const auto& upgrades = _Details::GetUpgradeCosts<T>();
        if(!upgrades.contains(t.Id)) return {};
        const auto& levelCosts = upgrades.at(t.Id);
        if(!levelCosts.contains(level)) {
            return {};
        }

        return levelCosts.at(level);
    }

    template<UpgradableType T>
    bool CanUpgrade(const T& upgradable, const ResourceCollection& resources) {
        auto costs = TryGetCost(upgradable);
        if(costs.has_value()) {
            return !resources.AreAnyLessThan(costs.value());
        }
        return false;
    }

    template<UpgradableType T>
    void Upgrade(T& upgradable, ResourceCollection& resources) {
        auto cost = TryGetCost(upgradable);
        if(!cost.has_value()) return;

        resources -= cost.value();
        auto before = upgradable.Describe();
        if constexpr(CountEnum<typename T::LevelType>) {
            upgradable.Level = Enum::Increment(upgradable.Level);
        } else {
            ++upgradable.Level;
        }
        upgradable.OnUpgrade();
        auto after = upgradable.Describe();
        auto description = std::format("Upgraded from {} to {}", before, after);
        Log::Info(description);
        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<UpgradeEvent<T>>>();
        pubSub.Publish({upgradable, description});
    }
} // namespace UpgradeManager

namespace UpgradeManager::_Details {
    inline std::vector<std::function<void()>>& GetInitFns() {
        static std::vector<std::function<void()>> fns{};
        return fns;
    }
} // namespace UpgradeManager::_Details