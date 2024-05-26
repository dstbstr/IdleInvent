#include "InventLib/Technology/Technology.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/DesignPatterns/PubSub.h"
#include "InventLib/Mechanics/Unlockable.h"
#include "InventLib/Mechanics/Purchasable.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/Resources/Resource.h"

namespace Invent {
	namespace Technologies {
		void Initialize() {
			auto& services = ServiceLocator::Get();
			auto& unlockables = services.GetOrCreate<std::unordered_map<std::string, Unlockable>>();
			services.CreateIfMissing<std::unordered_map<std::string, Purchasable>>();
			services.CreateIfMissing<PubSub<InventionLevel>>();

			for (auto& unlockable : Technologies::StoneAgeTechs) {
				unlockables.insert({ unlockable.Name, unlockable });
			}
		}
	}
}