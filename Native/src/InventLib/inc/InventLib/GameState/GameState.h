#pragma once

#include "InventLib/GameState/GameTime.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"
#include "InventLib/Resources/StorageCollection.h" // TODO: Combine with storage?


namespace Invent {
	struct GameState {
		GameState();

		void Tick(BaseTime elapsed);

		ResourceCollection CurrentResources{};
		StorageCollection CurrentStorages{};
		BaseTime TimeElapsed{};
	};
}