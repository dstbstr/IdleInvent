#include "InventLib/Mechanics/ScaleFunc.h"

#include "CommonTest.h"
namespace Invent {
    TEST(Scale, Constant_WithAnyLevel_ReturnsConstant) {
        auto costs = Scale::Constant<int, 10>;
        ASSERT_EQ(costs(0), 10);
        ASSERT_EQ(costs(10), 10);
        ASSERT_EQ(costs(200), 10);
    }

    TEST(Scale, Linear_WithBase10AndLevel3_Returns30) { 
        auto scale = Scale::Linear<int, 10>;
        ASSERT_EQ(scale(3), 30); 
    }

    TEST(Scale, Logarithmic_WithBase2AndLevel3_Returns8) { 
        auto scale = Scale::Logarithmic<int, 2>;
        ASSERT_EQ(scale(3), 8); 
    }

    TEST(Scale, Logarithmic_WithBase10AndLevel3_Returns1000) { 
        auto scale = Scale::Logarithmic<int, 10>;
        ASSERT_EQ(scale(3), 1'000); 
    }

    TEST(Scale, Punctuated_WithStep2AndLinearBase10AndLevel3_Returns20) {
        std::function<size_t(size_t)> costs = Scale::MakePunctuated<int, 2>(Scale::Linear<int, 10>);
        ASSERT_EQ(costs(3), 20); // 10, 10, 20, 20, 30, 30
    }

    TEST(Scale, Specified_WithValues_ReturnsValuesInOrder) {
        auto costs = Scale::Specified<int, 3, 5, 7>;
        ASSERT_EQ(costs(0), 3);
        ASSERT_EQ(costs(1), 5);
        ASSERT_EQ(costs(2), 7);
    }

    TEST(Scale, FixedPointLog_WithLog123AndPoint100AndLevel30_Returns1Point23ToThe30) {
        auto expected = static_cast<size_t>(std::pow(1.23, 30));
        auto scale = Scale::FixedPointLog<size_t, 123, 100>;
        ASSERT_EQ(expected, scale(30));
	}

    TEST(Scale, LogPunctuated_WithSpecifiedValueAndLogBase2_ReturnsSpecifiedValuesOnPowersOfTwo) {
        auto fn = Scale::MakeLogPunctuated<int, 2>(Scale::Specified<int, 3, 4, 5>);
        ASSERT_EQ(fn(1), 3);
        ASSERT_EQ(fn(2), 4);
        ASSERT_EQ(fn(4), 5);
	}

    TEST(Scale, LogPunctuated_WithSpecifiedValueAndLogBase10_RetursSpecifiedValueOnPowersOf10) {
        auto fn = Scale::MakeLogPunctuated<int, 10>(Scale::Specified<int, 3, 4, 5>);
		ASSERT_EQ(fn(1), 3);
		ASSERT_EQ(fn(10), 4);
		ASSERT_EQ(fn(100), 5);
    }
} // namespace Invent