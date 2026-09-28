#include "Walker/Home/HomeBase.h"

#include <Utilities/EnumUtils.h>

namespace Walker {
	void HomeBase::Tick(BaseTime elapsed) {
		Crew.Tick(elapsed);
	}

	EndpointKind HomeBase::GetMaxScoutKind() const {
		return Enum::Increment(FurthestEndpoint);
	}
}