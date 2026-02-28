#include "Constexpr/ConstexprSaveUtils.h"

#include <Platform/NumTypes.h>
#include <limits>

static_assert(Constexpr::Decompress<u64>(0) == 0);
static_assert(Constexpr::Compress(0) == 0);

inline constexpr auto TargetCompression = 0.2;
inline constexpr u64 MinTest = 100;
inline constexpr u64 MaxTest = 1'000'000'000;

constexpr bool TestCompressDecompress(u64 val) {
    auto target = static_cast<u64>(static_cast<f64>(val) * TargetCompression);
	return val - Constexpr::Decompress<u64>(Constexpr::Compress(val)) < target;
}

constexpr bool TestCompressRange(u64 min, u64 max) {
	for(auto i = min; i <= max; i *= 10) {
		if(!TestCompressDecompress(i)) return false;
	}
	return true;
}
static_assert(TestCompressRange(MinTest, MaxTest));
