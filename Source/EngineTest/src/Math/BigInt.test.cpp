#include "Math/BigInt.h"

#include <cmath>

TEST(BigIntTest, Log10Ratio_WithEqualVals_ReturnsZero) {
	ASSERT_EQ(BigInt::Log10Ratio(BigInt{ 42 }, BigInt{ 42 }), 0.0);
}

TEST(BigIntTest, Log10Ratio_WithPowersOfTen_ReturnsExpDiff) {
	ASSERT_NEAR(BigInt::Log10Ratio(BigInt{ 1000 }, BigInt{ 10 }), 2.0, 0.0001);
	ASSERT_NEAR(BigInt::Log10Ratio(BigInt{ 10 }, BigInt{ 1000 }), -2.0, 0.0001);
}

TEST(BigIntTest, Log10Ratio_FractionalRatio_DoesNotTruncate) {
	ASSERT_NEAR(BigInt::Log10Ratio(BigInt{ 3 }, BigInt{ 2 }), std::log10(1.5), 0.0001);
	ASSERT_NEAR(BigInt::Log10Ratio(BigInt{ 2 }, BigInt{ 3 }), std::log10(1.5), 0.0001);
}

TEST(BigIntTest, Log10Ratio_WithHugeValues_DoesNotOverflow) {
	auto a = BigInt::FromScientific(3, 1000);
	auto b = BigInt::FromScientific(2, 999);
	ASSERT_NEAR(BigInt::Log10Ratio(a, b), std::log10(15), 0.0001);
}

TEST(BigIntTest, Log10Ratio_BeyondDoubleRange_RemainsFinite) {
	auto a = BigInt::Pow10(1000);
	ASSERT_NEAR(BigInt::Log10Ratio(a, 1), 1000.0, 0.0001);
	ASSERT_NEAR(BigInt::Log10Ratio(1, a), 1000.0, 0.0001);
}

TEST(BigIntTest, Log10Ratio_WithTwoNegatives_ReturnsPositive) {
	ASSERT_NEAR(BigInt::Log10Ratio(BigInt{ -1000 }, BigInt{ -10 }), 2.0, 0.0001);
}

TEST(BigIntTest, Log10Ratio_WithMismatchedSigns_Throws) {
	ASSERT_THROW(BigInt::Log10Ratio(BigInt{ -1 }, BigInt{ 1 }), std::domain_error);
	ASSERT_THROW(BigInt::Log10Ratio(BigInt{ 1 }, BigInt{ -1 }), std::domain_error);
}

TEST(BigIntTest, Log10Ratio_WithZero_Throw) {
	ASSERT_THROW(BigInt::Log10Ratio(BigInt{ 0 }, BigInt{ 1 }), std::domain_error);
	ASSERT_THROW(BigInt::Log10Ratio(BigInt{ 1 }, BigInt{ 0 }), std::domain_error);
}

TEST(BigIntTest, FloatPower_WithDifferentExponents_ReturnsDifferentSizes) {
	BigInt a{10};
	BigInt b = a;

	ASSERT_TRUE(a.Pow(1.3f) < b.Pow(1.4f));
}

TEST(BigIntTest, FloatPower_WithZero_ReturnsOne) {
	BigInt a{ 10 };
	a.Pow(0.0f);
	ASSERT_EQ(a, BigInt{ 1 });
}

TEST(BigIntTest, FloatPower_WithOne_ReturnsSame) {
	BigInt a{ 10 };
	a.Pow(1.0f);
	ASSERT_EQ(a, BigInt{ 10 });
}

TEST(BigIntTest, FloatPower_WithOneBase_ReturnsOne) {
	BigInt a{ 1 };
	a.Pow(3.4f);
	ASSERT_EQ(a, BigInt{ 1 });
}

TEST(BigIntTest, FloatPower_WithNegativeBase_Throws) {
	BigInt a{ -10 };
	EXPECT_THROW(a.Pow(1.2f), std::domain_error);
}

TEST(BigIntTest, FloatPower_WithNegativeExponent_Throws) {
	BigInt a{10};
	EXPECT_THROW(a.Pow(-1.0f), std::domain_error);
}

TEST(BigIntTest, FloatPower_WithNaN_Throws) {
	BigInt a{ 10 };
	EXPECT_THROW(a.Pow(std::numeric_limits<float>::quiet_NaN()), std::domain_error);
}

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