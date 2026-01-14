#include "Platform/Graphics.h"
#include "GhostHunter/GhostHunterGame.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Inventory/Inventory.h"

#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/Sale.h>
#include <Utilities/IRandom.h>

namespace GhostHunter {
    bool GhostHunterGame::Initialize() { 
        auto& services = ServiceLocator::Get();

        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<GameSettings>();
        auto& resources = services.GetOrCreate<ResourceCollection>();
        resources = CreateRc<ResourceName>();
        resources.at(ResourceName::Cash).Current = 150;
        services.CreateIfMissing<PubSub<Purchase<ToolName>>>();
        services.CreateIfMissing<PubSub<Sale<Media>>>();
        services.CreateIfMissing<Market>();

        Inventory::Initialize();
        Tools::Initialize();

        return Ui::Initialize(); 
    }

    void GhostHunterGame::ShutDown() { 
        Ui::ShutDown(); 
        Inventory::ShutDown();
    }

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick([[maybe_unused]] BaseTime elapsed) { 
        auto& market = ServiceLocator::Get().GetRequired<Market>();
        market.Update(elapsed);
        Graphics::Render(Ui::Render); 
    }
} // namespace GhostHunter
