#include "Walker/WalkerGame.h"
#include "Walker/Ui/WalkerLayout.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Endpoints.h"
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
        
        auto& home = services.GetOrCreate<HomeBase>();
        home.Vehicles.Add(VehicleKind::Foot);
        home.Vehicles.Select(VehicleKind::Foot);
		home.Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));

        services.CreateIfMissing<PubSub<VehicleChanged>>();
        services.CreateIfMissing<PubSub<Phase>>();

        TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
            if(auto* journey = ServiceLocator::Get().Get<Journey>()) {
                journey->Tick(elapsed);
            }
        });

        //// cheats
        services.GetRequired<HomeBase>().TotalCrew = 10;
        home.Vehicles.Add(VehicleKind::Jet);
        home.Funds.Add(Money::Pow10(24));
        home.Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::InState));
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
