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
#include <optional>
#include <span>
#include <vector>

namespace Walker {
	struct HomeBase {
		HomeBase();

		TechManager Tech{};
		WalkerRates Rates;
		CrewManager Crew;

		Wallet Funds{};
		Garage Vehicles{};

		std::span<const std::unique_ptr<EndpointInstance>> GetEndpoints() const;
		size_t GetAvailableEndpointSlots() const;
		bool TryAddEndpoint(EndpointKind kind);
		std::optional<size_t> RemoveEndpoint(u64 id);

		EndpointKind FurthestEndpoint{EndpointKind::Unset};
		EndpointKind GetMaxScoutKind() const;

		bool TryHireCrew(u64 count = 1);

		void Tick(BaseTime elapsed);
		void Rebirth();
		void Prestiege();
		void Ascend();

	private:
		std::vector<std::unique_ptr<EndpointInstance>> m_Endpoints{};
		size_t m_MaxEndpointSlots{50};
	};
}