#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/WalkerRates.h"
#include "Walker/Crew/CrewManager.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Home/Garage.h"
#include "Walker/Home/Wallet.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/Vehicle.h"

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>

#include <memory>
#include <vector>

namespace Walker {
	struct HomeBase {
		HomeBase();

		TechManager Tech{};
		WalkerRates Rates;
		CrewManager Crew;

		Wallet Funds{};
		Garage Vehicles{};

		std::vector<std::unique_ptr<EndpointInstance>> Endpoints{};
		EndpointKind FurthestEndpoint{EndpointKind::Unset};
		EndpointKind GetMaxScoutKind() const;

		bool TryHireCrew(u64 count = 1);

		void Tick(BaseTime elapsed);
		void Rebirth();
		void Prestiege();
		void Ascend();
	};
}