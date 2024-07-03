#pragma once

#include "InventLib/GameState/GameTime.h"
#include "InventLib/GameState/SaveState.h"

#include "Core/DesignPatterns/ServiceLocator.h"

namespace Invent::EntryPoint {
	void Initialize();
    void ShutDown();
    
    void Load(const SaveState& save);
    void Save(SaveState& save);
    void Reset();

	void Tick(BaseTime elapsed);
}