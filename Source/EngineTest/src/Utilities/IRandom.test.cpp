#include "CommonTest.h"

#include "Utilities/IRandom.h"

#include <Platform/NumTypes.h>
#include <unordered_set>

TEST(DefaultRandomTest, GetNext_100Times_ProducesAtLeast90Numbers) {
	std::unordered_set<s64> seen;

	DefaultRandom random{};
    const u32 iterations = 100;
	for (u32 i = 0; i < iterations; i++) {
		seen.insert(random.GetNext());
	}

	const f32 expectedPercent = 0.9f; // Expect at least 90% unique values
    const size_t expectedMinCount = static_cast<size_t>(iterations * expectedPercent);
	ASSERT_GE(seen.size(), expectedMinCount);
}

TEST(DefaultRandomTest, GetNextU_100Times_ProducesAtLeast90Numbers) {
    std::unordered_set<u64> seen;

    DefaultRandom random{};
    const u32 iterations = 100;
    for(u32 i = 0; i < iterations; i++) {
        seen.insert(random.GetNextU());
    }

    const f32 expectedPercent = 0.9f; // Expect at least 90% unique values
    const size_t expectedMinCount = static_cast<size_t>(iterations * expectedPercent);
    ASSERT_GE(seen.size(), expectedMinCount);
}

TEST(DefaultRandomTest, GetNextMinMax_With25And100_ReturnsBetween25And100) {
    DefaultRandom random{};
    const u32 iterations = 100;
    const s64 minValue = 25;
    const s64 maxValue = 100;

    for(auto i = 0u; i < iterations; i++) {
        auto value = random.GetNext(minValue, maxValue);
        ASSERT_TRUE(value >= minValue && value < maxValue);

        auto valueU = random.GetNextU(static_cast<u64>(minValue), static_cast<u64>(maxValue));
        ASSERT_TRUE(valueU >= static_cast<u64>(minValue) && valueU < static_cast<u64>(maxValue));
    }
}

TEST(DefaultRandomTest, GetNextFloat_ReturnsBetween0And1) { 
    DefaultRandom random{};
    const u32 iterations = 100;
    for(auto i = 0u; i < iterations; i++) {
        auto value = random.GetNextFloat();
        ASSERT_TRUE(value >= 0.0f && value < 1.0f);
    }
}