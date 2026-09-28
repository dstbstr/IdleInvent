#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Home/CrewManager.h"
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
		CrewManager Crew{};

		Wallet Funds{};
		Garage Vehicles{};

		std::vector<std::unique_ptr<EndpointInstance>> Endpoints{};
		EndpointKind FurthestEndpoint{EndpointKind::Unset};
		EndpointKind GetMaxScoutKind() const;

		void Tick(BaseTime elapsed);
	};
}