#include "GhostHunter/Constants.h"
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

#include <Platform/Graphics.h>
#include <Manage/EventManager.h>
#include <Manage/TickManager.h>
#include <Mechanics/Purchasable.h>
#include <Mechanics/Sale.h>
#include <Utilities/IRandom.h>

void InitializePurchases();
void InitializeUpgrades();
void InitializeSales();

namespace {
    std::vector<ScopedHandle> m_Handles;
}

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
        InitializeSales();

        Unlocks unlocks{
            .BestTool = Constants::Unlocks::BestTool,
            .BestLocation = Constants::Unlocks::BestLocation,
            .BestMember = Constants::Unlocks::BestMember,
            .ShowRemainingResources = Constants::Unlocks::ShowRemainingResources,
            .StartingCash = Constants::Unlocks::StartingCash,
            .DecayMultiplier = Constants::Unlocks::DecayMultiplier
        };
        services.CreateIfMissing<Life>(unlocks);

        services.GetRequired<PubSub<EventStart>>().Subscribe(m_Handles, [](const EventStart& event) {
            Log::Info("Event Start: " + event.Event->Describe());
        });
        services.GetRequired<PubSub<EventEnd>>().Subscribe(m_Handles, [](const EventEnd& event) {
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
        TickManager::Get().Tick(elapsed);

        Graphics::Render(Ui::Render); 
    }
} // namespace GhostHunter
