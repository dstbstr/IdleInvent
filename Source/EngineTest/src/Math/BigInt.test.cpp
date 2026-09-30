#include "Math/BigInt.h"

TEST(BigIntTest, ScaleByPower_With0Exp_ReturnsSame) {
	BigInt a{1234};
	a.ScaleByPower(42.0, 0);
	ASSERT_EQ(a, BigInt{ 1234 });
}

TEST(BigIntTest, ScaleByPower_With0Lhs_Returns0) {
	BigInt a{0};
	a.ScaleByPower(1.2, 3);
	ASSERT_EQ(a, BigInt{ 0 });
}

TEST(BigIntTest, ScaleByPower_With1Growth_ReturnsSame) {
	BigInt a{ 1234 };
	a.ScaleByPower(1.0, 42);
	ASSERT_EQ(a, BigInt{ 1234 });
}

TEST(BigIntTest, ScaleByPower_WithPositiveGrowth_GetsLarger) {
	BigInt a{ 1234 };
	a.ScaleByPower(1.2, 10);
	ASSERT_TRUE(a > BigInt{ 1234 });
}

TEST(BigIntTest, ScaleByPower_WithLessThan1Growth_GetsSmaller) {
	BigInt a{ 1234 };
	a.ScaleByPower(0.8, 10);
	ASSERT_TRUE(a < BigInt{ 1234 });
}

TEST(BigIntTest, ScaleByPower_FractionalBaseLargeExponent_IsNearActual) {
	BigInt a{1};
	a.ScaleByPower(1.2, 300);

	auto expected = std::pow(1.2, 300);
	ASSERT_NEAR(BigInt::Ratio(a, BigInt{ 1 }) / expected, 1.0, 0.01);
}

TEST(BigIntTest, ScaleByPower_ResultBeyondDouble_CanBeCalculated) {
	BigInt a{1};
	a.ScaleByPower(10.0, 400);
	ASSERT_EQ(a, BigInt::Pow10(400));
}

TEST(BigIntTest, ScaleByPower_ResultLessThan1_RoundsToZero) {
	BigInt a{1};
	a.ScaleByPower(0.1, 20);
	ASSERT_EQ(a, BigInt{ 0 });
}

TEST(BigIntTest, ScaleByPower_ZeroBase_Throws) {
	BigInt a{ 1 };
	EXPECT_THROW(a.ScaleByPower(0.0, 42), std::domain_error);
}

TEST(BigIntTest, ScaleByPower_NegativeBase_Throws) {
	BigInt a{ 1 };
	EXPECT_THROW(a.ScaleByPower(-1.0, 42), std::domain_error);
}

TEST(BigIntTest, ScaleByPower_NanBase_Throws) {
	BigInt a{ 1 };
	EXPECT_THROW(a.ScaleByPower(std::numeric_limits<double>::quiet_NaN(), 42), std::domain_error);
}

TEST(BigIntTest, ScaleByPower_InfiniteBase_Throws) {
	BigInt a{ 1 };
	EXPECT_THROW(a.ScaleByPower(std::numeric_limits<double>::infinity(), 42), std::domain_error);
}