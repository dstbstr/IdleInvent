#pragma once

#include "InventLib/GameState/GameTime.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace Invent::EntryPoint {
	void Initialize();
	void Tick(BaseTime elapsed);
}