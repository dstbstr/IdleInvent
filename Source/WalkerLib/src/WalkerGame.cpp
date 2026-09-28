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
        
        auto& home = services.GetOrCreate<HomeBase>();
        home.Vehicles.Add(VehicleKind::Foot);
        home.Vehicles.Select(VehicleKind::Foot);
		home.Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));

        services.CreateIfMissing<PubSub<VehicleChanged>>();
        services.CreateIfMissing<PubSub<Phase>>();

        TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
            auto& services = ServiceLocator::Get();
            auto* journey = services.Get<Journey>();
            if(!journey) return;
            journey->Tick(elapsed);
            if(journey->GetPhase() != Phase::Complete) return;

			auto& home = services.GetRequired<HomeBase>();
			auto* endpoint = journey->GetEndpoint();
			home.FurthestEndpoint = std::max(home.FurthestEndpoint, endpoint->Kind);
            auto endpointId = endpoint->Id;

            services.Reset<Journey>();
			std::erase_if(home.Endpoints, [endpointId](const auto& e) { return e->Id == endpointId; });
        });

		TickManager::Get().Register(GlobalSubs, [](BaseTime elapsed) {
			ServiceLocator::Get().GetRequired<HomeBase>().Tick(elapsed);
		});

        // TODO: Move this somewhere
		home.Crew.Subscribe(GlobalSubs, [](const JobCompleted& job) {
			if (job.Role == CrewRole::Scout) {
				auto& home = ServiceLocator::Get().GetRequired<HomeBase>();
                auto endpoint = static_cast<EndpointKind>(job.CompletedKind);

				home.Endpoints.push_back(std::make_unique<EndpointInstance>(endpoint));
			}
		});

        //// cheats
        home.Crew.Add(9);
        
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
