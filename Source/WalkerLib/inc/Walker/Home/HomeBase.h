#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Home/CrewRoster.h"
#include "Walker/Home/Garage.h"
#include "Walker/Home/Wallet.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Travel/Vehicle.h"

#include <Platform/NumTypes.h>

#include <memory>
#include <vector>

namespace Walker {
	struct HomeBase {
		CrewRoster Crew{};

		Wallet Funds{};
		Garage Vehicles{};

		std::vector<std::unique_ptr<EndpointInstance>> Endpoints{};
	};
}