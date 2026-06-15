#pragma once

#include "GameState/GameTime.h"

struct GameStateSave {
    u32 ElapsedSeconds{0}; // max value = 4294967295 seconds = 1193 hours = 49 days
    u32 RunElapsedSeconds{0};
};

struct GameState {
    GameState() = default;
    virtual ~GameState() = default;
    GameState(const GameState&) = delete;
    GameState& operator=(const GameState&) = delete;
    GameState(GameState&&) = delete;
    GameState& operator=(GameState&&) = delete;

    void Load(const GameStateSave& save);
    void Save(GameStateSave& save) const;

	virtual void Tick(BaseTime elapsed);

	BaseTime TimeElapsed{};
	BaseTime CurrentRunElapsed{};
};
