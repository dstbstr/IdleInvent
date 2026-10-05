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

namespace {
    std::vector<ScopedHandle> GlobalSubs{};
}

namespace Walker {
    bool WalkerGame::Initialize() {
        auto& services = ServiceLocator::Get();
        services.CreateIfMissing<TickManager>();
        services.SetThisAsThat<DefaultRandom, IRandom>();
        services.CreateIfMissing<std::unordered_map<std::string, Animation>>();
        
        services.CreateIfMissing<WalkerSettings>();
        auto& home = services.GetOrCreate<HomeBase>();
        home.Rebirth();

        services.CreateIfMissing<PubSub<VehicleChanged>>();
        services.CreateIfMissing<PubSub<Phase>>();

        TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
            auto& services = ServiceLocator::Get();
			auto& home = services.GetRequired<HomeBase>();
            auto* journey = services.Get<Journey>();
            if(!journey) {
                auto* vehicle = home.Vehicles.GetSelected();
				if (vehicle && home.GetEndpoints().size() > 0) {
					services.Set<Journey>(vehicle, home.GetEndpoints()[0].get(), home);
				}
                return;
            }

            journey->Tick(elapsed);
            if(journey->GetPhase() != Phase::Complete) return;

			auto* endpoint = journey->GetEndpoint();
			home.FurthestEndpoint = std::max(home.FurthestEndpoint, endpoint->Kind);
            auto endpointId = endpoint->Id;
            if(auto index = home.RemoveEndpoint(endpoint->Id)) {
                services.Reset<Journey>();
                if(home.GetEndpoints().empty()) return;

			    index = std::min(*index, home.GetEndpoints().size() - 1);
			    services.Set<Journey>(home.Vehicles.GetSelected(), home.GetEndpoints()[*index].get(), home);
            }
        });

		TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
			ServiceLocator::Get().GetRequired<HomeBase>().Tick(elapsed);
		});

        // TODO: Move this somewhere
		home.Crew.Subscribe(GlobalSubs, [](const JobCompleted& job) {
			if (job.Role == CrewRole::Scout) {
				auto& home = ServiceLocator::Get().GetRequired<HomeBase>();
                auto endpoint = static_cast<EndpointKind>(job.CompletedKind);

                home.TryAddEndpoint(endpoint);
			}
		});

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
        GlobalSubs.clear();
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
                TickManager::Get().Tick(step);
                remaining -= step;
            }
        }

        Graphics::Render(WalkerUi::Layout::Render);
    }
}
