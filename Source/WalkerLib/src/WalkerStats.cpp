#include "Walker/WalkerStats.h"

#include <algorithm>

namespace Walker {
	const WalkerStats& StatManager::SinceRebirth() const { return m_RebirthStats; }
	const WalkerStats& StatManager::SincePrestiege() const { return m_PrestiegeStats; }
	const WalkerStats& StatManager::SinceAscend() const { return m_AscendStats; }
	const WalkerStats& StatManager::AllTime() const { return m_AllTimeStats; }

	void StatManager::Tick(BaseTime elapsed) {
		if (elapsed <= ZeroTime) return;

		Visit([elapsed](WalkerStats& stats) {
			stats.PlayTime += elapsed;
		});
	}

	void StatManager::OnCrewHired(u64 count) {
		Visit([count](WalkerStats& stats) {
			stats.TotalCrewHired += count;
		});
	}

	void StatManager::OnCrewFound(u64 count) {
		Visit([count](WalkerStats& stats) {
			stats.TotalCrewFound += count;
		});
	}

	void StatManager::OnTravel(Distance distance, Mass fuelBurned) {
		Visit([distance, fuelBurned](WalkerStats& stats) {
			stats.TotalDistance += distance;
			stats.TotalFuelBurned += fuelBurned;
		});
	}

	void StatManager::OnArrival(EndpointKind kind, Distance distanceFromHome) {
		Visit([kind, distanceFromHome](WalkerStats& stats) {
			stats.FurthestEndpointDistance = std::max(stats.FurthestEndpointDistance, distanceFromHome);
			stats.FurthestEndpointReached = std::max(stats.FurthestEndpointReached, kind);
		});
	}

	void StatManager::OnCargoDelivered(Mass cargoDelivered) {
		Visit([cargoDelivered](WalkerStats& stats) {
			stats.TotalCargoDelivered += cargoDelivered;
		});
	}

	void StatManager::OnRoundTripComplete() {
		Visit([](WalkerStats& stats) {
			stats.CompletedRoundTrips++;
		});
	}

	void StatManager::OnEndpointComplete() {
		Visit([](WalkerStats& stats) {
			stats.CompletedEndpoints++;
		});
	}

	void StatManager::Rebirth() {
		m_RebirthStats = {};
	}

	void StatManager::Prestiege() {
		m_PrestiegeStats = {};
		Rebirth();
	}

	void StatManager::Ascend() {
		m_AscendStats = {};
		Prestiege();
	}

	void StatManager::Visit(std::function<void(WalkerStats& stats)> fn) {
		fn(m_RebirthStats);
		fn(m_PrestiegeStats);
		fn(m_AscendStats);
		fn(m_AllTimeStats);
	}
}