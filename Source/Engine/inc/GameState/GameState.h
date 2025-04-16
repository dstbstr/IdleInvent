#pragma once

#include "GameState/GameTime.h"

struct GameStateSave {
    u32 ElapsedSeconds{0}; // max value = 4294967295 seconds = 1193 hours = 49 days
    u32 RunElapsedSeconds{0};
};

struct GameState {
    void Load(const GameStateSave& save);
    void Save(GameStateSave& save) const;

	void Tick(BaseTime elapsed);

	BaseTime TimeElapsed{};
	BaseTime CurrentRunElapsed{};
};
