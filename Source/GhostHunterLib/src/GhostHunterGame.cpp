#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/GameState/Life.h"
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

void InitializePurchases();
void InitializeUpgrades();

namespace GhostHunter {
    bool GhostHunterGame::Initialize() { 
        auto& services = ServiceLocator::Get();
        EventManager::Initialize();
        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<GameSettings>();
        Market::Initialize();
        InitializePurchases();
        InitializeUpgrades();

        auto& inv = services.GetOrCreate<Life>().GetInventory();
        inv.Resources = CreateRc<ResourceName>(std::pair{ResourceName::Cash, 150});

        services.GetRequired<PubSub<EventStart>>().Subscribe([](const EventStart& event) {
            Log::Info("Event Start: " + event.Event->Describe());
        });
        services.GetRequired<PubSub<EventEnd>>().Subscribe([](const EventEnd& event) {
            Log::Info("Event End: " + event.Event->Describe());
        });
        return Ui::Initialize(); 
    }

    void GhostHunterGame::ShutDown() { 
        Ui::ShutDown(); 
        //Locations::ShutDown();
        //Tools::ShutDown();
        Market::ShutDown();
    }

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick(BaseTime elapsed) {
        auto& services = ServiceLocator::Get();
        services.GetRequired<Life>().Update(elapsed);
        services.GetRequired<EventManager>().Update(elapsed);

        Graphics::Render(Ui::Render); 
    }
} // namespace GhostHunter
