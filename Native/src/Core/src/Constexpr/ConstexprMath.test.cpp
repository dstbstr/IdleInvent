#include "Core/Constexpr/ConstexprMath.h"

static_assert(Constexpr::Pow(2, 0) == 1);
static_assert(Constexpr::Pow(2, 1) == 2);
static_assert(Constexpr::Pow(2, 2) == 4);
static_assert(Constexpr::Pow(2, 3) == 8);