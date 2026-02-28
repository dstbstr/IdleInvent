#pragma once

#include <Platform/NumTypes.h>

enum struct PurchaseAmount : u8 { One, Ten, Half, Max };

size_t GetPurchaseCount(size_t count, PurchaseAmount amount);