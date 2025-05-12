#include "TechMaze/TechMazeGame.h"
#include "TechMaze/Ui/Ui.h"
#include "Platform/Graphics.h"

namespace TechMaze {
    bool TechMazeGame::Initialize() {
        return Ui::Initialize();
    }

    void TechMazeGame::ShutDown() {
    }

    void TechMazeGame::LoadGame() {
    }

    void TechMazeGame::SaveGame() {
    }

    void TechMazeGame::DeleteGame() {
    }

    void TechMazeGame::Tick([[maybe_unused]] BaseTime elapsed) {
        Graphics::Render(Ui::Render);
    }
}
