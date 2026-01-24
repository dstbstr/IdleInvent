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

template<typename E>
concept UpgradeEnum = CountEnum<E>;

template<typename E>
concept LevelEnum = CountEnum<E>;

template<typename T>
concept UpgradableType = requires(T t) {
    typename T::TypeEnum;
    typename T::LevelEnum;
    { t.OnUpgrade() };
    { t.Describe() } -> std::convertible_to<std::string>;
    { t.Id } -> std::convertible_to<typename T::TypeEnum>;
    { t.Level } -> std::convertible_to<typename T::LevelEnum>;
};

template<UpgradableType T>
struct UpgradeEvent {
    const T& Upgrade;
    std::string Description;
};

namespace UpgradeManager {
    namespace _Details {
        template<UpgradableType T>
        std::unordered_map<typename T::TypeEnum, std::unordered_map<typename T::LevelEnum, ResourceCollection>>&
        GetUpgradeCosts() {
            static std::unordered_map<typename T::TypeEnum, std::unordered_map<typename T::LevelEnum, ResourceCollection>> costs;
            return costs;
        }
    } // namespace _Details

    template<UpgradableType T>
    std::optional<ResourceCollection> TryGetCost(const T& t) {
        auto current = t.Level;
        auto nextLevel = Enum::Increment(current);
        if(nextLevel == current) return {};

        const auto& upgrades = _Details::GetUpgradeCosts<T>();
        if(!upgrades.contains(t.Id)) return {};
        const auto& levelCosts = upgrades.at(t.Id);
        if(!levelCosts.contains(nextLevel)) {
            return {};
        }

        return levelCosts.at(nextLevel);
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