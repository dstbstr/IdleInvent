#include "GameState/GameState.h"

void GameState::Load(const GameStateSave& save) {
	TimeElapsed = save.ElapsedSeconds * OneSecond;
	CurrentRunElapsed = save.RunElapsedSeconds * OneSecond;
}

void GameState::Save(GameStateSave& save) const {
	save.ElapsedSeconds = static_cast<u32>(TimeElapsed / OneSecond);
    save.RunElapsedSeconds = static_cast<u32>(CurrentRunElapsed / OneSecond);
}

void GameState::Tick(BaseTime elapsed) {
    TimeElapsed += elapsed;
	CurrentRunElapsed += elapsed;
}
