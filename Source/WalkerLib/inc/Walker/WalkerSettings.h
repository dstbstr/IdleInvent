#pragma once

#include <Settings/PurchaseAmount.h>

namespace Walker {
	struct WalkerSettings {
		PurchaseAmount PurchaseSetting{PurchaseAmount::One};
	};
}