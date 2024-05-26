#pragma once

#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"
#include "InventLib/Resources/StorageCollection.h" // TODO: Combine with storage?

namespace Invent {
	struct GameState {
		GameState();

		void Tick();

		ResourceCollection CurrentResources{};
		StorageCollection CurrentStorages{};

		size_t ElapsedTicks{ 0 };
	};
}