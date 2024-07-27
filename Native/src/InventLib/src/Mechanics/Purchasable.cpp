#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/GameState/GameState.h"

#include <optional>

namespace {
	std::optional<Invent::Purchasable> ToPurchase;
}

namespace Invent {
	namespace Purchasables {
		void Tick() {
			if (ToPurchase.has_value()) {
                //auto& gameState = ServiceLocator::Get().GetRequired<GameState>();
				//ToPurchase->Purchase(gameState.CurrentLife.Resources);
				ToPurchase.reset();
			}
		}

		void TryPurchase(const std::string& name) {
			auto& services = ServiceLocator::Get();
			auto& purchasables = services.GetRequired<std::unordered_map<std::string, Purchasable>>();

			if (purchasables.contains(name)) {
                /*
				auto& purchasable = purchasables.at(name);
				Log::Info("Trying to purchase: " + name);
				if (purchasable.CanPurchase(services.GetRequired<GameState>().CurrentLife.Resources)) {
					ToPurchase = purchasable;
					Log::Info("Purchase successful");
				} else {
					Log::Info("Purchase failed");
				}
				*/
			}
		}
	}
}