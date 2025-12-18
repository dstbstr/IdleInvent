#pragma once

#include <Game.h>
#include <Platform/Platform.h>

namespace GhostHunter {
    struct GhostHunterGame : public ::Game {
        GhostHunterGame(::Platform& platform) : Game(platform) {}

        bool Initialize() override;
        void ShutDown() override;

        void LoadGame() override;
        void SaveGame() override;
        void DeleteGame() override;

        void Tick(BaseTime elapsed) override;
    };
} // namespace GhostHunter