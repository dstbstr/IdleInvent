#include "GhostHunter/GameState/GameSettings.h"
#include "GhostHunter/GameState/Life.h"
#include "GhostHunter/GameState/World.h"
#include "GhostHunter/GhostHunterGame.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Investigation/Investigation.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Media/Media.h"
#include "GhostHunter/Media/Market.h"
#include "GhostHunter/Resources/GhostHunterResources.h"
#include "GhostHunter/Ui/Ui.h"

#include <Platform/Graphics.h>
#include <Manage/EventManager.h>
#include <Manage/TickManager.h>
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
        services.CreateIfMissing<TickManager>();
        Market::Initialize();
        Investigation::Initialize();
        InitializePurchases();
        InitializeUpgrades();
        InitializeWorld();

        Unlocks unlocks{
            .BestTool = ToolName::EmfDetector,
            .BestLocation = LocationName::House,
            .BestMember = MemberName::You,
            .ShowRemainingResources = false,
            .StartingCash = 150,
            .DecayMultiplier = 0.9
        };
        services.CreateIfMissing<Life>(unlocks);

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
        Market::ShutDown();
    }

    void GhostHunterGame::LoadGame() {}

    void GhostHunterGame::SaveGame() {}

    void GhostHunterGame::DeleteGame() {}

    void GhostHunterGame::Tick(BaseTime elapsed) {
        ServiceLocator::Get().GetRequired<TickManager>().Tick(elapsed);

        Graphics::Render(Ui::Render); 
    }
} // namespace GhostHunter
