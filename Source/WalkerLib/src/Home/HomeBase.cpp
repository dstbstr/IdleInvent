#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() 
		: Rates(Tech, Milestones)
		, Crew(Rates, Tech)
		, OfflineTime(Rates) {
		ServiceLocator::Get().GetRequired<PubSub<PhaseChanged>>().Subscribe(m_Subs, [this](const PhaseChanged& change) {
			auto* journey = ServiceLocator::Get().Get<Journey>();
			if (!journey) return;

			using enum Phase;
			using enum MilestoneTriggerEvent;
			auto Send = [&](MilestoneTriggerEvent trigger) { Milestones.OnEvent(trigger, *this, journey); };

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

		Rebirth();
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
		auto& services = ServiceLocator::Get();
		auto* journey = services.Get<Journey>();
		if (!journey) {
			auto* vehicle = Vehicles.GetSelected();
			if (vehicle && m_Endpoints.size() > 0) {
				services.Set<Journey>(vehicle, m_Endpoints.at(0).get(), *this);
			}
			return;
		}

		journey->Tick(elapsed);
		if (journey->GetPhase() != Phase::Complete) return;

		auto* endpoint = journey->GetEndpoint();
		FurthestEndpoint = std::max(FurthestEndpoint, endpoint->Kind);
		auto endpointId = endpoint->Id;
		services.Reset<Journey>();
		if (auto index = RemoveEndpoint(endpointId)) {
			if (m_Endpoints.empty()) return;

			index = std::min(*index, m_Endpoints.size() - 1);
			services.Set<Journey>(Vehicles.GetSelected(), m_Endpoints.at(*index).get(), *this);
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

	void HomeBase::Rebirth() {
		FurthestEndpoint = EndpointKind::Unset;
		Funds.Reset();
		Vehicles.Reset();
		m_Endpoints.clear();
		m_Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));
		Crew.Rebirth();
		Stats.Rebirth();
	}

	void HomeBase::Prestiege() {
		Tech.Reset();
		Crew.Prestiege();
		Rates.Prestiege();
		Stats.Prestiege();
		Rebirth();
	}

	void HomeBase::Ascend() {
		Rates.Ascend();
		Stats.Ascend();
		Prestiege();
	}
}