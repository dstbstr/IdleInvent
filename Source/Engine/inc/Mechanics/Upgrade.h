#pragma once

#include "Resources/Resource.h"
#include <string>

struct IUpgradable {
    IUpgradable() = default;
	virtual ~IUpgradable() = default;
	virtual void OnUpgrade() = 0;
    virtual ResourceCollection GetUpgradeCost() const = 0;
    virtual std::string Describe() const = 0;
    virtual bool IsMaxLevel() const = 0;
};

struct UpgradeEvent {
    const IUpgradable* Upgradable;
    std::string Description;
};

namespace UpgradeManager {
    void Upgrade(IUpgradable& upgradable, ResourceCollection& resources);
    bool CanUpgrade(const IUpgradable& upgradable, const ResourceCollection& resources);
}