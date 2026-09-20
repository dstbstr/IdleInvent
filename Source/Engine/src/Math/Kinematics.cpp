#include "Math/Kinematics.h"
#include "Math/FixedInt.h"
#include "Math/BigInt.h"

static_assert(DistanceForSpeedChange(10, 10, 0) == 0);
static_assert(TimeForSpeedChange(10, 10, 0) == 0);

// starting from rest
static_assert(DistanceForSpeedChange(0, 10, 2) == 25);
static_assert(TimeForSpeedChange(0, 10, 2) == 5);

// stopping
static_assert(DistanceForSpeedChange(10, 0, 2) == 25);
static_assert(TimeForSpeedChange(10, 0, 2) == 5);

// nonzero starting speed and ending speed
static_assert(DistanceForSpeedChange(6, 10, 2) == 16);
static_assert(TimeForSpeedChange(6, 10, 2) == 2);
static_assert(DistanceForSpeedChange(10, 6, 2) == 16);
static_assert(TimeForSpeedChange(10, 6, 2) == 2);

// time scale
static_assert(TimeForSpeedChange(0, 1, 2, 1000) == 500);
static_assert(TimeForSpeedChange(1, 0, 2, 1000) == 500);

// floating point
static_assert(DistanceForSpeedChange<double>(0.0, 1.0, 2.0) == 0.25);
static_assert(TimeForSpeedChange<double>(0.0, 1.0, 2.0) == 0.5);

// FixedInt
static_assert(DistanceForSpeedChange<u128>(10, 0, 2) == u128(25));
static_assert(TimeForSpeedChange<u128>(10, 0, 2) == u128(5));

// BigInt
static_assert(DistanceForSpeedChange<BigInt>(10, 0, 2) == BigInt(25));
static_assert(TimeForSpeedChange<BigInt>(10, 0, 2) == BigInt(5));

// PeakSquared
static_assert(PeakSpeedSquaredForDistance(50, 0, 0, 2) == 100);
// 16 distance from 6 to 10, then 9 distance braking from 10 to 8
static_assert(PeakSpeedSquaredForDistance(25, 6, 8, 2) == 100);
static_assert(PeakSpeedSquaredForDistance(25, 8, 6, 2) == 100);

// 0
static_assert(PeakSpeedSquaredForDistance(0, 0, 0, 2) == 0);
static_assert(PeakSpeedSquaredForDistance(0, 6, 6, 2) == 36);

// float
static_assert(PeakSpeedSquaredForDistance<double>(0.5, 0.0, 0.0, 0.5) == 0.25);

// custom types
static_assert(PeakSpeedSquaredForDistance<u128>(50, 0, 0, 2) == u128(100));
static_assert(PeakSpeedSquaredForDistance<BigInt>(50, 0, 0, 2) == BigInt(100));

// really big values
static_assert(PeakSpeedSquaredForDistance<BigInt>(BigInt::Pow10(40), 0, 0, 2) == BigInt::FromScientific(2, 40));
