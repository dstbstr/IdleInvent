#pragma once

#include <Platform/NumTypes.h>

// prestiege is the second layer rebirth, provides exponents. Milestone based
namespace Walker {
	struct WalkerPrestiege {
		u64 AvailablePoints;

		u64 CargoWorkPoints{};
		u64 JobWorkPoints{};
		u64 AccelPoints{};
		u64 MaxSpeedPoints{};
		u64 MaxCapacityPoints{};

		void Reset() {
			*this = {};
		}
	};
}