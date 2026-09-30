#pragma once

#include <Platform/NumTypes.h>
#include <Instrumentation/Logging.h>

enum struct PurchaseAmount : u8 { One, Ten, Half, Max };

template<typename TCount>
constexpr TCount GetPurchaseCount(TCount count, PurchaseAmount amount) {
	switch(amount) {
		using enum PurchaseAmount;
		case One: return TCount{1};
		case Ten: return count >= TCount{ 10 } ? TCount{ 10 } : TCount{0};
		case Half: return count > TCount{ 1 } ? count / TCount{ 2 } : TCount{ 1 };
		case Max: return count;
		default:
			DR_ASSERT_MSG(false, "Invalid purchase amount");
			return TCount{};
	}
}