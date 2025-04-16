#include "Constexpr/ConstexprMath.h"

static_assert(Constexpr::Clamp(3, 0, 5) == 3);
static_assert(Constexpr::Clamp(3, 0, 2) == 2);
static_assert(Constexpr::Clamp(3, 4, 5) == 4);

static_assert(Constexpr::Clamp(0.3, 0.0, 1.0) == 0.3);
static_assert(Constexpr::Clamp(0.3, 0.0, 0.2) == 0.2);
static_assert(Constexpr::Clamp(0.3, 0.4, 1.0) == 0.4);

static_assert(Constexpr::Pow(2, 0) == 1);
static_assert(Constexpr::Pow(2, 1) == 2);
static_assert(Constexpr::Pow(2, 2) == 4);
static_assert(Constexpr::Pow(2, 3) == 8);

constexpr bool IsNear(auto lhs, auto rhs) { return Constexpr::Abs(lhs - rhs) < 0.01; }

static_assert(Constexpr::FloatToPercent(1.0) == 0.0);
static_assert(IsNear(Constexpr::FloatToPercent(1.1), 10.0), "1.1 should be a 10% increase");
static_assert(IsNear(Constexpr::FloatToPercent(0.9), 10.0), "0.9 should be a 10% decrease");
static_assert(IsNear(Constexpr::FloatToPercent(0.0), 100.0), "0.0 should be a 100% decrease");
static_assert(IsNear(Constexpr::FloatToPercent(2.0), 100.0), "2.0 should be a 100% increase");