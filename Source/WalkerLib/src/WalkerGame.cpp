#include "Walker/WalkerGame.h"
#include "Walker/Ui/WalkerLayout.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Travel/Vehicle.h"

#include "Animation/Animation.h"
#include "DesignPatterns/ServiceLocator.h"
#include "Manage/TickManager.h"
#include "Platform/Graphics.h"

#include <DesignPatterns/PubSub.h>
#include <Utilities/IRandom.h>

namespace {
    std::vector<ScopedHandle> GlobalSubs{};
}

namespace Walker {
    bool WalkerGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();
        services.CreateIfMissing<OwnedVehicle>(VehicleKind::Foot);
        services.CreateIfMissing<HomeBase>();
        services.CreateIfMissing<PubSub<VehicleChanged>>();
        services.CreateIfMissing<PubSub<Phase>>();

        TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
            if(auto* journey = ServiceLocator::Get().Get<Journey>()) {
                journey->Tick(elapsed);
            }
        });

        //// cheats
        services.GetRequired<HomeBase>().TotalCrew = 10;
        //services.Reset<OwnedVehicle>();
        //services.Set<OwnedVehicle>(VehicleKind::Jet);

        ////

        return WalkerUi::Layout::Initialize();
    }

    void WalkerGame::ShutDown() {
        WalkerUi::Layout::ShutDown();
        GlobalSubs.clear();
    }

    void WalkerGame::LoadGame() {}

    void WalkerGame::SaveGame() {}

    void WalkerGame::DeleteGame() {}

    void WalkerGame::Tick(BaseTime elapsed) {
        TickManager::Get().Tick(elapsed);
        Graphics::Render(WalkerUi::Layout::Render);
    }
}
