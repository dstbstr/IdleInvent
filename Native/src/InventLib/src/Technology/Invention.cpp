#include "InventLib/Technology/Invention.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Resources/Resource.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"

#include <array>

namespace {
	constexpr const std::array LevelSuffixes = { "I", "II", "III" "IV", "V" };

	size_t UnlockedTechCount = 0;
	auto ResourceSpecialty = Invent::ResourceName::Unset;
}


namespace Invent {
	InventionLevel::InventionLevel(ResourceName resource, size_t age, size_t level, std::string name, std::string description, const std::vector<Effect>& effects, std::function<void()> extraOnUnlock, std::function<void()> extraOnPurchase)
		: Name(name + " " + LevelSuffixes[level])
		, Description(description)
		, UnlockTechCount((age * 5) + (level * 2))
		, PurchaseResourceCosts(GetResourceCosts(resource, (age * 3) + level))
		, UnlockResourceCosts(PurchaseResourceCosts / 2)
		, Effects(effects) 
		, ExtraOnUnlock(extraOnUnlock)
		, ExtraOnPurchase(extraOnPurchase)
	{}

	Unlockable Invention::MakeUnlockable(size_t level) const {
		const auto& currentLevel = Levels[level];
		auto CanUnlock = [currentLevel]() {
			auto& gameState = ServiceLocator::Get().GetRequired<GameState>();

			return currentLevel.UnlockTechCount <= UnlockedTechCount && currentLevel.UnlockResourceCosts <= gameState.CurrentResources;
		};

		auto OnUnlock = [this, currentLevel, level]() -> std::vector<Unlockable> {
			auto& purchasables = ServiceLocator::Get().GetRequired<std::unordered_map<std::string, Purchasable>>();
			auto purchasable = MakePurchasable(level);
			purchasables.emplace(purchasable.Name, purchasable);
			UnlockedTechCount++;
			ResourceSpecialty = Resource;

			if (currentLevel.ExtraOnUnlock) currentLevel.ExtraOnUnlock();

			if (level < Levels.size() - 1) {
				return { MakeUnlockable(level + 1) };
			}
			else {
				return {};
			}
		};

		return Unlockable(currentLevel.Name, CanUnlock, OnUnlock);
	}

	Purchasable Invention::MakePurchasable(size_t level) const {
		const auto& currentLevel = Levels[level];

		auto OnPurchase = [currentLevel]() {
			auto& ps = ServiceLocator::Get().GetRequired<PubSub<InventionLevel>>();
			ps.Publish(currentLevel);

			if (currentLevel.ExtraOnPurchase) currentLevel.ExtraOnPurchase();
		};

		return Purchasable(currentLevel.Name, currentLevel.PurchaseResourceCosts, OnPurchase);
	}
}