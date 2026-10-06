#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/WalkerRates.h"
#include "Walker/Crew/CrewManager.h"
#include "Walker/Crew/WalkerTech.h"
#include "Walker/Home/Garage.h"
#include "Walker/Home/TimeBank.h"
#include "Walker/Home/Wallet.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Milestones/Milestones.h"
#include "Walker/Milestones/WalkerStats.h"
#include "Walker/Travel/Vehicle.h"

#include <GameState/GameTime.h>
#include <Platform/NumTypes.h>
#include <Utilities/Handle.h>

#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace Walker {
	struct HomeBase {
		HomeBase();

		TechManager Tech{};
		MilestoneManager Milestones{};
		WalkerRates Rates;
		CrewManager Crew;
		TimeBank OfflineTime;

		Wallet Funds{};
		Garage Vehicles{};
		StatManager Stats{};

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
		std::vector<ScopedHandle> m_Subs{};
		size_t m_MaxEndpointSlots{50};

		void TickJourney(BaseTime elapsed);
	};
}