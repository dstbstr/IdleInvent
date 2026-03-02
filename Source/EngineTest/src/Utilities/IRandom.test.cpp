#include "CommonTest.h"

#include "Utilities/IRandom.h"

#include <Platform/NumTypes.h>
#include <unordered_set>

TEST(DefaultRandom, GetNext_100Times_ProducesAtLeast90Numbers) {
	std::unordered_set<u64> seen;

	DefaultRandom random{};
    const u32 iterations = 100;
    const u64 maxValue = 1'000'000ull;
	for (u32 i = 0; i < iterations; i++) {
		seen.insert(random.GetNextU(0, maxValue));
	}

	const f32 expectedPercent = 0.9f; // Expect at least 90% unique values
    const size_t expectedMinCount = static_cast<size_t>(iterations * expectedPercent);
	ASSERT_GE(seen.size(), expectedMinCount);
}