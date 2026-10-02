#include "Walker/Home/HomeBase.h"

#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() : Rates(Tech), Crew(Rates, Tech) {}

	void HomeBase::Tick(BaseTime elapsed) {
		Crew.Tick(elapsed);
	}

	EndpointKind HomeBase::GetMaxScoutKind() const {
		return Enum::Increment(FurthestEndpoint);
	}

	bool HomeBase::TryHireCrew(u64 count) {
		auto cost = Rates.GetHireCost(Crew.GetHiredCount(), count);
		if(Funds.Spend(cost)) {
			Crew.Hire(count);
			return true;
		}

		return false;
	}

	void HomeBase::Rebirth() {
		FurthestEndpoint = EndpointKind::Unset;
		Funds.Reset();
		Vehicles.Reset();
		Endpoints.clear();
		Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));
		Crew.Rebirth();
	}

	void HomeBase::Prestiege() {
		Tech.Reset();
		Crew.Prestiege();
		Rates.Prestiege();
		Rebirth();
	}

	void HomeBase::Ascend() {
		Rates.Ascend();
		Prestiege();
	}
}