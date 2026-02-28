#include "Settings/PurchaseAmount.h"
#include "Instrumentation/Logging.h"

#include <algorithm>

namespace {
    constexpr size_t TEN = 10;
}
size_t GetPurchaseCount(size_t count, PurchaseAmount amount)
{
    switch(amount) {
    case PurchaseAmount::One: return std::min(size_t(1), count);
    case PurchaseAmount::Ten: return count >= TEN ? TEN : 0;
    case PurchaseAmount::Half: return count / 2;
    case PurchaseAmount::Max: return count;
    default: DR_ASSERT_MSG(false, "Invalid PurchaseAmount"); return 0;
    }
}
