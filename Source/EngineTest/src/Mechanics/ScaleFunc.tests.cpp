#include "CommonTest.h"
#include "Mechanics/ScaleFunc.h"

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

TEST(Scale, Exp_WithBase2AndLevel3_Returns8) { 
    auto scale = Scale::Exp<int, 2>;
    ASSERT_EQ(scale(3, 1), 8); 
}

TEST(Scale, Exp_WithBase10AndLevel3_Returns1000) { 
    auto scale = Scale::Exp<int, 10>;
    ASSERT_EQ(scale(3, 1), 1'000); 
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

TEST(Scale, FixedPointExp_WithExp123AndPoint100AndLevel30_Returns1Point23ToThe30) {
    auto expected = static_cast<size_t>(std::pow(1.23, 30));
    auto scale = Scale::FixedPointExp<size_t, 123, 100>;
    ASSERT_EQ(expected, scale(30, 1));
}

TEST(Scale, ExpPunctuated_StepSize5StepGrowth10Linear3_ProducesGroupsOf5GrowingBy10) {
    auto fn = Scale::MakeExpPunctuated<int, 5, 10>(Scale::Linear<int, 3>);
    std::vector<int> expected{3, 6, 9, 12, 15, 30, 60, 90, 120, 150, 300, 600, 900, 1200, 1500};
    std::vector<int> actual;
	for (size_t i = 0; i < expected.size(); ++i) {
		actual.emplace_back(fn(i));
    }
    ASSERT_EQ(expected, actual);
}
