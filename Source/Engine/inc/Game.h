#pragma once

#include "GameState/GameTime.h"

#include <Platform/Platform.h>
#include <Utilities/Handle.h>

struct Game {
    Game(Platform& platform) : Platform(platform) {}
    virtual ~Game() = default;

    virtual bool Initialize() = 0;
    virtual void ShutDown() = 0;

    virtual void LoadGame() = 0;
    virtual void SaveGame() = 0;
    virtual void DeleteGame() = 0;

    virtual void Tick(BaseTime elapsed) = 0;

    Platform& Platform;

protected:
    std::vector<ScopedHandle> m_Handles;
};