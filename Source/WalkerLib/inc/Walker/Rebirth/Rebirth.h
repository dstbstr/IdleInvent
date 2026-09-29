#pragma once
#include "Walker/Rebirth/Rebirth.h"
#include "Walker/WalkerUnits.h"

#include <Platform/NumTypes.h>

// first level rebirth provides multipliers, milestone based
namespace Walker {
	struct WalkerRebirth {
		u64 AvailablePoints{0};

		Quantity CargoWorkRateMultiplier{1};
		Quantity JobWorkRateMultiplier{1};
		Quantity MaxSpeedMultiplier{1};
		Quantity MaxCapacityMultiplier{1};
	};
}