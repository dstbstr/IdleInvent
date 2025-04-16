#pragma once
#include "InventLib/Resources/InventResources.h"

#include <Mechanics/Effect.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/Unlockable.h>

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Invent {
	struct InventionLevel {
		InventionLevel(ResourceName resource, size_t age, size_t level, std::string name, std::string description, std::vector<std::string> prereq, const std::vector<Effect>& effects, std::function<void()> extraOnUnlock = {}, std::function<void()> extraOnPurchase = {});

		std::string Name{};
		ResourceName Resource{ ResourceName::Unset };
        size_t Age{0};
        size_t Level{0};
		std::string Description{};
		std::vector<std::string> Prerequisites{};
		ResourceCollection PurchaseResourceCosts{};
		ResourceCollection UnlockResourceCosts{};
		std::vector<Effect> Effects{};

		std::function<void()> ExtraOnUnlock{};
		std::function<void()> ExtraOnPurchase{};
	};

	/*
	struct Invention {
		std::string Name{};
		ResourceName Resource{ ResourceName::Unset };
		std::vector<InventionLevel> Levels{};

		Unlockable MakeUnlockable(size_t level) const;
		Purchasable MakePurchasable(size_t level) const;
	};

	namespace Inventions {
        void Reset();
    } // namespace Inventions
	*/
}
