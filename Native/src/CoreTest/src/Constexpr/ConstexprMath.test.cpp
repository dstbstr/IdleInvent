#include "Core/Constexpr/ConstexprMath.h"

static_assert(Constexpr::Clamp(3, 0, 5) == 3);
static_assert(Constexpr::Clamp(3, 0, 2) == 2);
static_assert(Constexpr::Clamp(3, 4, 5) == 4);

static_assert(Constexpr::Clamp(0.3, 0.0, 1.0) == 0.3);
static_assert(Constexpr::Clamp(0.3, 0.0, 0.2) == 0.2);
static_assert(Constexpr::Clamp(0.3, 0.4, 1.0) == 0.4);