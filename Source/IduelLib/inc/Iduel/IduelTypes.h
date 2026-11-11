#pragma once

#include <Math/BigInt.h>

namespace Iduel {
    using Quantity = BigIntImpl<40, 23, true>;
    static constexpr Quantity Zero{0};
	using Money = Quantity;
}