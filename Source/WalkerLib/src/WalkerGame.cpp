#include "Walker/WalkerGame.h"
#include "Walker/WalkerSettings.h"
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

#include <algorithm>

namespace Walker {
    bool WalkerGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();
        
        services.CreateIfMissing<PubSub<VehicleChanged>>();
        services.CreateIfMissing<PubSub<PhaseChanged>>();
        services.CreateIfMissing<WalkerSettings>();
        auto& home = services.GetOrCreate<HomeBase>();

        //// cheats
        home.Crew.Add(9);
        
        home.Vehicles.Add(VehicleKind::Jet);
        home.Funds.Add(Money::Pow10(24));
        home.TryAddEndpoint(EndpointKind::InState);
        home.Rates.GetRebirth().AvailablePoints = 100;
		home.Rates.GetPrestiege().AvailablePoints = 100;
        home.Rates.GetAscend().AvailablePoints = 100;
        home.OfflineTime.AddTime(OneHour * 8);
        ////

        return WalkerUi::Layout::Initialize();
    }

    void WalkerGame::ShutDown() {
        WalkerUi::Layout::ShutDown();
    }

    void WalkerGame::LoadGame() {}

    void WalkerGame::SaveGame() {}

    void WalkerGame::DeleteGame() {}

    void WalkerGame::Tick(BaseTime elapsed) {
        constexpr BaseTime::rep MaxSteps = 8;
        if(elapsed > ZeroTime) {
			auto& home = ServiceLocator::Get().GetRequired<HomeBase>();
            home.Stats.Tick(elapsed);

            auto remaining = home.OfflineTime.Spend(elapsed);
            auto stepSize = std::max(BaseTime{20}, BaseTime{(remaining.count() + MaxSteps - 1) / MaxSteps});

            while(remaining > ZeroTime) {
                auto step = std::min(remaining, stepSize);
                home.Tick(step);
                TickManager::Get().Tick(step);
                remaining -= step;
            }
            home.Milestones.UpdateMilestones(home.Stats.AllTime());
        }

        Graphics::Render(WalkerUi::Layout::Render);
    }
}
