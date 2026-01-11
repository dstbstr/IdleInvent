#include "Platform/Graphics.h"
#include "GhostHunter/GhostHunterGame.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"

#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>
#include <Utilities/IRandom.h>

namespace GhostHunter {
    bool GhostHunterGame::Initialize() { 
        auto& services = ServiceLocator::Get();

        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<GameSettings>();
        services.CreateIfMissing<GhostHunterResources>();
        services.CreateIfMissing<PubSub<Media>>();
        services.CreateIfMissing<Market>();

        return Ui::Initialize(); 
    }

    void GhostHunterGame::ShutDown() { Ui::ShutDown(); }

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick([[maybe_unused]] BaseTime elapsed) { Graphics::Render(Ui::Render); }
} // namespace GhostHunter
