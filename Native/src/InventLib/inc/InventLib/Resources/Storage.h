#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Mechanics/Progression.h"
#include "InventLib/Mechanics/Advancement.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {
	struct Storage {
		Storage(ResourceName resourceType, Advancement advance) : ResourceType(resourceType), ResourcesToLevel{GetResourceCosts(resourceType, 1)}, Advance(advance) {}
		
		ResourceName ResourceType{ ResourceName::Unset };
		ResourceCollection ResourcesToLevel{};

		size_t CurrentLevel{1};
        Advancement Advance{"Unset", AdvancementCosts::Linear<2>, {}};

		bool CanLevelUp() const;
		void LevelUp();
		void Tick(BaseTime elapsed);
	};
}