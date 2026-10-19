#include "Math/Kinematics.h"
#include "Math/BigInt.h"

#include <cmath>

TEST(LinearDrag, MotionWithDrag_IsLessThanMotionWithout) {
	auto speed = 2.0;
	auto accel = 3.0;
	auto elapsed = 4.0;

	auto result = IntegrateLinearDrag(speed, accel, 2.0, elapsed);

	auto noDragSpeed = speed + accel * elapsed;
	auto noDragDistance = speed * elapsed + 0.5 * accel * elapsed * elapsed;

	ASSERT_TRUE(result.EndSpeed > 0.0);
	ASSERT_TRUE(result.EndSpeed < noDragSpeed);
	ASSERT_TRUE(result.Distance > 0.0);
	ASSERT_TRUE(result.Distance < noDragDistance);
}

TEST(LinearDrag, MoreDrag_ReducesSpeedAndDistanceMore) {
	auto speed = 2.0;
	auto accel = 3.0;
	auto elapsed = 4.0;

	auto weakDrag = IntegrateLinearDrag(speed, accel, 1.5, elapsed);
	auto strongDrag = IntegrateLinearDrag(speed, accel, 1.0, elapsed);

	ASSERT_TRUE(weakDrag.EndSpeed > strongDrag.EndSpeed);
	ASSERT_TRUE(weakDrag.Distance > strongDrag.Distance);
}

TEST(LinearDrag, LinearDrag_WithOnes_ReturnsSaneNumbers) {
	auto [speed, dist] = IntegrateLinearDrag(0.0, 1.0, 1.0, 1.0);
	auto decay = std::exp(-1.0);
	ASSERT_NEAR(speed, 1.0 - decay, 1e-6);
	ASSERT_NEAR(dist, decay, 1e-6);
}

TEST(LinearDrag, Milliseconds_MatchSeconds) {
	auto seconds = IntegrateLinearDrag(0.0, 1.0, 1.0, 1.0);
	auto milliseconds = IntegrateLinearDrag(0.0, 1.0, 1000.0, 1000.0, 1000.0);

	ASSERT_NEAR(seconds.EndSpeed, milliseconds.EndSpeed, 1e-6);
	ASSERT_NEAR(seconds.Distance, milliseconds.Distance, 1e-6);
}

TEST(LinearDrag, AtTerminalSpeed_RemainsConstant) {
	// Terminal speed = accel * response Time = 2 * 5
	auto result = IntegrateLinearDrag(10.0, 2.0, 5.0, 3.0);

	ASSERT_NEAR(result.EndSpeed, 10.0, 1e-10);
	ASSERT_NEAR(result.Distance, 30.0, 1e-10);
}

TEST(LinearDrag, Braking_ReducesSpeedBeforeStopping) {
	auto result = IntegrateLinearDrag(2.0, -1.0, 1.0, 1.0);

	ASSERT_NEAR(result.EndSpeed, 0.1036383235, 1e-9);
	ASSERT_NEAR(result.Distance, 0.8963616765, 1e-9);
}

TEST(LinearDrag, SplitIntervals_MatchOneInterval) {
	auto whole = IntegrateLinearDrag(0.0, 1.0, 1.0, 1.0);
	auto speed = 0.0;
	auto dist = 0.0;

	for(int i = 0; i < 10; i++) {
		auto step = IntegrateLinearDrag(speed, 1.0, 1.0, 0.1);
		speed = step.EndSpeed;
		dist += step.Distance;
	}

	ASSERT_NEAR(speed, whole.EndSpeed, 1e-10);
	ASSERT_NEAR(dist, whole.Distance, 1e-10);
}

TEST(LinearDrag, BigInt_WithMilliseconds) {
	using T = BigIntImpl<40, 23, true>;
	auto result = IntegrateLinearDrag(T{ 0 }, T{ 1000 }, T{ 1000 }, T{ 1000 }, T{ 1000 });

	ASSERT_EQ(result.EndSpeed, T{632});
	ASSERT_EQ(result.Distance, T{367});
}