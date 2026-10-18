#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() 
		: Rates(Tech, Milestones)
		, Crew(Rates, Tech)
		, OfflineTime(Rates)
		, m_Journey(*this) {
		ServiceLocator::Get().GetRequired<PubSub<PhaseChanged>>().Subscribe(m_Subs, [this](const PhaseChanged& change) {
			using enum Phase;
			using enum MilestoneTriggerEvent;
			auto Send = [&](MilestoneTriggerEvent trigger) { Milestones.OnEvent(trigger, *this, &m_Journey); };

			if (change.To == Loading) {
				Send(EndpointReached);
			}
			else if (change.From == Unloading) {
				Send(RoundTripComplete);
				if (change.To == Complete) Send(EndpointComplete);
			}
		});

		Crew.Subscribe(m_Subs, [this](const JobCompleted& job) {
			if (job.Role == CrewRole::Scout) {
				auto endpoint = static_cast<EndpointKind>(job.CompletedKind);

				TryAddEndpoint(endpoint);
			}
		});

		RebirthReset();
	}

	std::span<const std::unique_ptr<EndpointInstance>> HomeBase::GetEndpoints() const {
		return m_Endpoints;
	}

	size_t HomeBase::GetAvailableEndpointSlots() const {
		return m_MaxEndpointSlots - m_Endpoints.size();
	}

	bool HomeBase::TryAddEndpoint(EndpointKind kind) {
		if (m_Endpoints.size() >= m_MaxEndpointSlots) return false;
		m_Endpoints.push_back(std::make_unique<EndpointInstance>(kind));
		return true;
	}

	std::optional<size_t> HomeBase::RemoveEndpoint(u64 id) {
		auto it = std::ranges::find_if(m_Endpoints, [id](const auto& e) { return e->Id == id; });
		if (it == m_Endpoints.end()) return std::nullopt;

		auto index = static_cast<size_t>(it - m_Endpoints.begin());
		m_Endpoints.erase(it);
		return index;
	}

	void HomeBase::Tick(BaseTime elapsed) {
		TickJourney(elapsed);
		Crew.Tick(elapsed, GetAvailableEndpointSlots());
	}

	void HomeBase::TickJourney(BaseTime elapsed) {
		m_Journey.Tick(elapsed);
		if (m_Journey.GetPhase() != Phase::Complete) return;

		auto* endpoint = m_Journey.GetEndpoint();
		FurthestEndpoint = std::max(FurthestEndpoint, endpoint->Kind);
		auto endpointId = endpoint->Id;
		if (auto index = RemoveEndpoint(endpointId)) {
			if (m_Endpoints.empty()) return;

			index = std::min(*index, m_Endpoints.size() - 1);
		}
	}

	EndpointKind HomeBase::GetMaxScoutKind() const {
		return Enum::Increment(FurthestEndpoint);
	}

	bool HomeBase::TryHireCrew(u64 count) {
		auto cost = Rates.GetHireCost(Crew.GetHiredCount(), count);
		if(Funds.Spend(cost)) {
			Crew.Hire(count);
			Stats.OnCrewHired(count);
			return true;
		}

		return false;
	}

	bool HomeBase::TryRebirth() {
		auto points = Rates.GetRebirthReward(Stats.SinceRebirth().TotalDistance);
		if(points == 0) return false;
		
		Rates.GetRebirth().AddPoints(points);
		RebirthReset();
		Stats.OnRebirth();
		return true;
	}

	bool HomeBase::TryPrestige() {
		auto points = Rates.GetPrestigeReward(Stats.SincePrestige().TotalDistance);
		if(points == 0) return false;

		Rates.GetPrestige().AddPoints(points);
		PrestigeReset();
		Stats.OnPrestige();
		return true;
	}

	bool HomeBase::TryAscend() {
		auto points = Rates.GetAscendReward(Stats.SinceAscend().TotalDistance);
		if (points == 0) return false;

		Rates.GetAscend().AddPoints(points);
		AscendReset();
		Stats.OnAscend();
		return true;
	}

	void HomeBase::RebirthReset() {
		ServiceLocator::Get().Reset<Journey>();
		FurthestEndpoint = EndpointKind::Unset;
		Funds.Reset();
		Vehicles.Reset();
		m_Endpoints.clear();
		m_Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));
		Crew.RebirthReset();
		Stats.RebirthReset();
	}

	void HomeBase::PrestigeReset() {
		Tech.Reset();
		Crew.PrestigeReset();
		Rates.PrestigeReset();
		Stats.PrestigeReset();
		RebirthReset();
	}

	void HomeBase::AscendReset() {
		Rates.AscendReset();
		Stats.AscendReset();
		PrestigeReset();
	}
}