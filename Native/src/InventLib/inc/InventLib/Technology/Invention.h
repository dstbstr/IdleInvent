#pragma once

#include "InventLib/Mechanics/Effect.h"
#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/Mechanics/Unlockable.h"

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace Invent {
	struct InventionLevel {
		InventionLevel(ResourceName resource, size_t age, size_t level, std::string name, std::string description, const std::vector<Effect>& effects, std::function<void()> extraOnUnlock = {}, std::function<void()> extraOnPurchase = {});

		std::string Name{};
		std::string Description{};
		size_t UnlockTechCount{};
		ResourceCollection PurchaseResourceCosts{};
		ResourceCollection UnlockResourceCosts{};
		std::vector<Effect> Effects{};

		std::function<void()> ExtraOnUnlock{};
		std::function<void()> ExtraOnPurchase{};
	};

	struct Invention {
		std::string Name{};
		ResourceName Resource{ ResourceName::Unset };
		std::vector<InventionLevel> Levels{};

		Unlockable MakeUnlockable(size_t level) const;
		Purchasable MakePurchasable(size_t level) const;
	};
}
