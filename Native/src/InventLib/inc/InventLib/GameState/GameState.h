#pragma once

#include "InventLib/GameState/GameTime.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"

namespace Invent {
	struct GameState {
		GameState();

		void Tick(BaseTime elapsed);

		ResourceCollection CurrentResources{};
		std::unordered_map<ResourceName, Storage> Storages{};
		BaseTime TimeElapsed{};
	};
}