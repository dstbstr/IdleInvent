#pragma once

#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Endpoints.h"

#include <GameState/GameTime.h>
#include <functional>

namespace Walker {
	struct WalkerStats {
		BaseTime PlayTime{};
		u64 TotalCrewHired{};
		u64 TotalCrewFound{};

		Distance TotalDistance{};
		Distance FurthestEndpointDistance{};
		EndpointKind FurthestEndpointReached{ EndpointKind::Unset };
		u64 CompletedRoundTrips{};
		u64 CompletedEndpoints{};

		Mass TotalCargoDelivered{};
		Mass TotalFuelBurned{};
	};

	class StatManager {
	public:
		const WalkerStats& SinceRebirth() const;
		const WalkerStats& SincePrestiege() const;
		const WalkerStats& SinceAscend() const;
		const WalkerStats& AllTime() const;

		void Tick(BaseTime elapsed);
		void OnCrewHired(u64 count);
		void OnCrewFound(u64 count);
		void OnTravel(Distance distance, Mass fuelBurned);
		void OnArrival(EndpointKind kind, Distance distanceFromHome);
		void OnCargoDelivered(Mass cargoDelivered);
		void OnRoundTripComplete();
		void OnEndpointComplete();

		void Rebirth();
		void Prestiege();
		void Ascend();

	private:
		WalkerStats m_RebirthStats{};
		WalkerStats m_PrestiegeStats{};
		WalkerStats m_AscendStats{};
		WalkerStats m_AllTimeStats{};

		void Visit(std::function<void(WalkerStats&)> visitor);
	};
}