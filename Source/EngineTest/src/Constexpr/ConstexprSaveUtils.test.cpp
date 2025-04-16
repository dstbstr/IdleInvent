#include "Constexpr/ConstexprSaveUtils.h"

#include <Platform/NumTypes.h>
#include <limits>

static_assert(Constexpr::Decompress<u64>(0) == 0);
static_assert(Constexpr::Compress(0) == 0);

constexpr bool TestCompressDecompress(u64 val) {
    auto target = static_cast<u64>(val * 0.2);
	return val - Constexpr::Decompress<u64>(Constexpr::Compress(val)) < target;
}

static_assert(TestCompressDecompress(100));
static_assert(TestCompressDecompress(1000));
static_assert(TestCompressDecompress(10'000));
static_assert(TestCompressDecompress(100'000));
static_assert(TestCompressDecompress(1'000'000));
static_assert(TestCompressDecompress(10'000'000));
static_assert(TestCompressDecompress(100'000'000));
static_assert(TestCompressDecompress(1'000'000'000));