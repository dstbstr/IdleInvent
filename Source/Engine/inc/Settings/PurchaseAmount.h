#pragma once

#include <stddef.h>

enum struct PurchaseAmount { One, Ten, Half, Max };

size_t GetPurchaseCount(size_t count, PurchaseAmount amount);