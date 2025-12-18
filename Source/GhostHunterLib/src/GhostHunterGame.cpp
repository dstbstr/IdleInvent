#include "Platform/Graphics.h"
#include "GhostHunter/GhostHunterGame.h"
#include "GhostHunter/Ui/Ui.h"

namespace GhostHunter {
    bool GhostHunterGame::Initialize() { return Ui::Initialize(); }

    void GhostHunterGame::ShutDown() {}

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick([[maybe_unused]] BaseTime elapsed) { Graphics::Render(Ui::Render); }
} // namespace GhostHunter
