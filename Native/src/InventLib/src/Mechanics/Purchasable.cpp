#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/GameState/GameState.h"

#include <optional>

namespace {
	std::optional<Invent::Purchasable> ToPurchase;

}
namespace Invent {
	namespace Purchasables {
		void Tick() {
			auto& gameState = ServiceLocator::Get().GetRequired<GameState>();
			if (ToPurchase.has_value()) {
				ToPurchase->Purchase(gameState.CurrentResources);
				ToPurchase.reset();
			}
		}

		void TryPurchase(const std::string& name) {
			auto& services = ServiceLocator::Get();
			auto& purchasables = services.GetRequired<std::unordered_map<std::string, Purchasable>>();

			if (purchasables.contains(name)) {
				auto& purchasable = purchasables.at(name);
				if (purchasable.CanPurchase(services.GetRequired<GameState>().CurrentResources)) {
					ToPurchase = purchasable;
				}
			}
		}
	}
}