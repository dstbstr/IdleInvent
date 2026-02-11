#pragma once
#include "GhostHunter/Locations/Locations.h"
#include "DesignPatterns/PubSub.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Utilities/EnumUtils.h"

namespace GhostHunter {
	struct World {
        std::unordered_map<LocationName, Location> Locations{};
	};

	inline void InitializeWorld() {
		auto& world = ServiceLocator::Get().GetOrCreate<World>();
        for(auto name = Enum::Begin<LocationName>(); name != Enum::End<LocationName>(); name = Enum::Increment(name)) {
			world.Locations.emplace(name, Location(name));
		}
	}
}