#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/GhostHunterGame.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Ui/Ui.h"

#include <DesignPatterns/ServiceLocator.h>
#include <DesignPatterns/PubSub.h>
#include "Platform/Graphics.h"
#include <Mechanics/EventManager.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/Sale.h>
#include <Utilities/IRandom.h>

namespace GhostHunter {
    bool GhostHunterGame::Initialize() { 
        auto& services = ServiceLocator::Get();
        EventManager::Initialize();
        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<GameSettings>();
        auto& resources = services.GetOrCreate<ResourceCollection>();
        resources = CreateRc<ResourceName>();
        resources.at(ResourceName::Cash).Current = 150;

        Market::Initialize();
        Tools::Initialize();
        Inventory::Initialize();
        Locations::Initialize();

        return Ui::Initialize(); 
    }

    void GhostHunterGame::ShutDown() { 
        Ui::ShutDown(); 
        Locations::ShutDown();
        Inventory::ShutDown();
        Tools::ShutDown();
        Market::ShutDown();
    }

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick(BaseTime elapsed) {
        auto& services = ServiceLocator::Get();
        services.GetRequired<Market>().Update(elapsed);
        if(auto* investigation = services.Get<Investigation>()) {
            investigation->Update(elapsed);
        }
        Tools::Update(elapsed);

        Graphics::Render(Ui::Render); 
    }
} // namespace GhostHunter
