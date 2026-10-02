#pragma once
#include "Walker/Rebirth/Rebirth.h"
#include "Walker/WalkerUnits.h"

#include <Platform/NumTypes.h>

// first level rebirth provides multipliers, milestone based
namespace Walker {
	struct WalkerRebirth {
		u64 AvailablePoints{0};

		u64 CargoWorkPoints{0};
		u64 JobWorkPoints{0};
		u64 AccelPoints{0};
		u64 MaxSpeedPoints{0};
		u64 MaxCapacityPoints{0};

		void Reset() {
			*this = {};
		}
	};
}