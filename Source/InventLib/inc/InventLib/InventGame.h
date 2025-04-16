#pragma once

#include <Game.h>
#include <Platform/Platform.h>

namespace Invent {
    struct InventGame : public ::Game {
        InventGame(::Platform& platform) : Game(platform) {}

        virtual bool Initialize();
        virtual void ShutDown();

        virtual void LoadGame();
        virtual void SaveGame();
        virtual void DeleteGame();

        virtual void Tick(BaseTime elapsed);
    };
}