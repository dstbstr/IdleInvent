#pragma once

#include "Core/NumTypes.h"
#include "InventLib/Resources/Resource.h"
#include "InventLib/Mechanics/Progression.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Invent {
	struct Storage {
		Storage(ResourceName resourceType, Progression progress) : ResourceType(resourceType), ExpProgress(progress) {}
		
		ResourceName ResourceType{ ResourceName::Unset };

		s64 CurrentLevel{ 0 };
		s64 CurrentExp{ 0 };
		s64 ExpToLevel{ 100 };

		ResourceCollection ResourcesToLevel{};

		Progression ExpProgress{};
		Progression TempExpProgress{};

		bool CanLevelUp() const;
		void LevelUp();
		void Tick();
	};
}