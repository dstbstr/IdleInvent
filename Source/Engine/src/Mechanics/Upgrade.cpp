#include "Mechanics/Upgrade.h"
#include "DesignPatterns/ServiceLocator.h"
#include "DesignPatterns/PubSub.h"

namespace UpgradeManager {
    void Upgrade(IUpgradable& upgradable, ResourceCollection& resources) {
        auto cost = upgradable.GetUpgradeCost();
        resources -= cost;

        auto before = upgradable.Describe();
        upgradable.OnUpgrade();
        auto after = upgradable.Describe();
        auto description = std::format("Upgraded from {} to {}", before, after);

        auto& pubSub = ServiceLocator::Get().GetRequired<PubSub<UpgradeEvent>>();
        pubSub.Publish({&upgradable, description});
    }

    bool CanUpgrade(const IUpgradable& upgradable, const ResourceCollection& resources) {
        if(upgradable.IsMaxLevel()) return false;
        auto cost = upgradable.GetUpgradeCost();
        if(resources.AreAnyLessThan(cost)) return false;
        return true;
    }
}