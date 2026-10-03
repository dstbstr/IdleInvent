#pragma once

#include <Platform/NumTypes.h>

namespace Walker {
	struct WalkerProgression {
		u64 AvailablePoints{0};

		u64 CargoWorkPoints{ 0 };
		u64 JobWorkPoints{ 0 };
		u64 AccelPoints{ 0 };
		u64 MaxSpeedPoints{ 0 };
		u64 MaxCapacityPoints{ 0 };

		void Reset() {
			*this = {};
		}
	};
}