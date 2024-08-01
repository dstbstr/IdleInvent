#pragma once

#include <stddef.h>

namespace Invent {
	enum struct PurchaseAmount { One, Ten, Half, Max };

	size_t GetPurchaseCount(size_t count, PurchaseAmount amount);
}
