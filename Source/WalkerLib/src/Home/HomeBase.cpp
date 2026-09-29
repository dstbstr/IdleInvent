#include "Walker/Home/HomeBase.h"

#include <Utilities/EnumUtils.h>

namespace Walker {
	HomeBase::HomeBase() : Crew(Rates) {}

	void HomeBase::Tick(BaseTime elapsed) {
		Crew.Tick(elapsed);
	}

	EndpointKind HomeBase::GetMaxScoutKind() const {
		return Enum::Increment(FurthestEndpoint);
	}

	void HomeBase::Rebirth() {
		FurthestEndpoint = EndpointKind::Unset;
		Funds.Reset();
		Vehicles.Reset();
		Endpoints.clear();
		Endpoints.push_back(std::make_unique<EndpointInstance>(EndpointKind::Neighborhood));
		Crew.Rebirth();
	}
}