#include "Walker/Home/HomeBase.h"

#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() : Rates(Tech), Crew(Rates, Tech) {}

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
		Stats.Tick(elapsed);
		Crew.Tick(elapsed, GetAvailableEndpointSlots());
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