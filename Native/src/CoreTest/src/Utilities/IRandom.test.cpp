#include "CommonTest.h"

#include "Core/Utilities/IRandom.h"
#include <unordered_set>

TEST(DefaultRandom, GetNext_100Times_ProducesAtLeast90Numbers) {
	std::unordered_set<size_t> seen;

	DefaultRandom random{};
	for (u32 i = 0; i < 100; i++) {
		seen.insert(random.GetNext(0, 1'000'000ull));
	}

	ASSERT_GE(seen.size(), 90);
}