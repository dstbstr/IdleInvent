#pragma once

#include "InventLib/GameState/GameTime.h"

#include "InventLib/Resources/Resource.h"
#include "InventLib/Resources/Storage.h"

namespace Invent {
	struct GameState {
		GameState();
        ~GameState();

		void Tick(BaseTime elapsed);

		ResourceCollection CurrentResources{};
		std::unordered_map<ResourceName, Storage> Storages{};
		BaseTime TimeElapsed{};

	private:
		size_t m_AchivementHandle{0};
        size_t m_TechHandle{0};
		size_t m_EventHandle{0};
	};
}