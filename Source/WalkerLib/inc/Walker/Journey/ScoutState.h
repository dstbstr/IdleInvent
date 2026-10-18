#pragma once
#include "Walker/Crew/JobProgress.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/WalkerUnits.h"

namespace Walker {
	struct ScoutState {
		EndpointKind Target{EndpointKind::Unset};
		JobProgress Progress{};
		Quantity WorkRemainder{};
	};
}