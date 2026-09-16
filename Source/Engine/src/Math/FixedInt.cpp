#include "Math/FixedInt.h"

static_assert(sizeof(UFixedInt<32>) == sizeof(u32));
static_assert(sizeof(UFixedInt<33>) == sizeof(u32) * 2);
static_assert(sizeof(SFixedInt<32>) == sizeof(u32));
static_assert(sizeof(SFixedInt<33>) == sizeof(u32) * 2);

// equality
static_assert(UFixedInt<96>{} == UFixedInt<2>{0});
static_assert(UFixedInt<96>{UFixedInt<12>{42}} == UFixedInt<96>{42});
static_assert(UFixedInt<2>(0) == UFixedInt<2>(0));
static_assert(UFixedInt<32>(42) == UFixedInt<24>(42));
static_assert(UFixedInt<32>(42) != UFixedInt<24>(43));
static_assert(UFixedInt<2>(3) != UFixedInt<5>(7)); // 3 == 11b, 7 == 111b, testing against truncation

static_assert(SFixedInt<96>{} == SFixedInt<2>{0});
static_assert(SFixedInt<96>{SFixedInt<12>{42}} == SFixedInt<96>{42});
static_assert(SFixedInt<8>{SFixedInt<5>{-1}} == SFixedInt<8>{-1});
static_assert(SFixedInt<96>(SFixedInt<33>{-42}) == SFixedInt<96>{-42});
static_assert(SFixedInt<2>(0) == SFixedInt<2>(0));
static_assert(SFixedInt<32>(42) == SFixedInt<24>(42));
static_assert(SFixedInt<32>(42) != SFixedInt<24>(43));
static_assert(SFixedInt<5>(-1) == SFixedInt<5>(-1));
static_assert(SFixedInt<5>(-1) != SFixedInt<5>(1));
static_assert(SFixedInt<5>(-1) == SFixedInt<64>(-1));

// comparison
static_assert(UFixedInt<12>(42) < UFixedInt<12>(43));
static_assert(UFixedInt<12>(43) > UFixedInt<12>(42));
static_assert(UFixedInt<12>(42) >= UFixedInt<24>(40));

static_assert(SFixedInt<12>(42) < SFixedInt<12>(43));
static_assert(SFixedInt<12>(43) > SFixedInt<12>(42));
static_assert(SFixedInt<12>(42) >= SFixedInt<24>(40));
static_assert(SFixedInt<12>(-42) < SFixedInt<12>(-40));
static_assert(SFixedInt<5>(-1) < SFixedInt<5>(1));
static_assert(SFixedInt<20>(-2) < SFixedInt<40>(-1));

// min/max
static_assert(UFixedInt<8>::MinValue == UFixedInt<8>(0));
static_assert(UFixedInt<8>::MaxValue == UFixedInt<8>(255));
static_assert(SFixedInt<8>::MinValue == SFixedInt<8>(-128));
static_assert(SFixedInt<8>::MaxValue == SFixedInt<8>(127));
static_assert(UFixedInt<96>::MaxValue == -UFixedInt<96>(1));

// unary minus
static_assert(-SFixedInt<8>(42) == SFixedInt<8>(-42));
static_assert(-SFixedInt<5>(0) == SFixedInt<5>(0));
static_assert(-SFixedInt<8>(-128) == SFixedInt<8>(-128)); // overflow, should remain the same
static_assert(-UFixedInt<8>(1) == UFixedInt<8>(255)); // underflow, should wrap around

// addition
static_assert(UFixedInt<8>(24) + UFixedInt<8>(24) == UFixedInt<8>(48));
static_assert(UFixedInt<8>(0) + UFixedInt<8>(32) == UFixedInt<8>(32));
static_assert(UFixedInt<8>::MaxValue + UFixedInt<8>(1) == UFixedInt<8>(0));
static_assert(UFixedInt<16>(10) + UFixedInt<8>(1) == UFixedInt<16>(11));

static_assert(SFixedInt<8>(24) + SFixedInt<8>(24) == SFixedInt<8>(48));
static_assert(SFixedInt<8>(0) + SFixedInt<8>(32) == SFixedInt<8>(32));
static_assert(SFixedInt<8>::MaxValue + SFixedInt<8>(1) == SFixedInt<8>::MinValue); // overflow, should wrap around
static_assert(SFixedInt<8>(-10) + SFixedInt<8>(5) == SFixedInt<8>(-5));
static_assert(SFixedInt<8>(-10) + SFixedInt<8>(-5) == SFixedInt<8>(-15));
static_assert(SFixedInt<8>::MinValue + SFixedInt<8>(-1) == SFixedInt<8>::MaxValue); // underflow, should wrap around
static_assert(SFixedInt<16>(10) + SFixedInt<8>(1) == SFixedInt<16>(11));

// subtraction
static_assert(SFixedInt<8>(3) - SFixedInt<8>(7) == SFixedInt<8>(-4));
static_assert(UFixedInt<8>(0) - UFixedInt<8>(1) == UFixedInt<8>::MaxValue);
static_assert(SFixedInt<8>::MinValue - SFixedInt<8>(1) == SFixedInt<8>::MaxValue);
static_assert(SFixedInt<16>(10) - SFixedInt<8>(1) == SFixedInt<16>(9));

// multiplication
static_assert(UFixedInt<8>(12) * UFixedInt<8>(3) == UFixedInt<8>(36));
static_assert(SFixedInt<8>(-7) * SFixedInt<8>(3) == SFixedInt<8>(-21));
static_assert(SFixedInt<8>(-7) * SFixedInt<8>(-3) == SFixedInt<8>(21));
static_assert(UFixedInt<5>(16) * UFixedInt<5>(2) == UFixedInt<5>(0)); // overflow, should wrap around

// division
static_assert(UFixedInt<8>(12) / UFixedInt<8>(3) == UFixedInt<8>(4));
static_assert(UFixedInt<8>(2) / UFixedInt<8>(3) == UFixedInt<8>(0)); // integer division
static_assert(SFixedInt<8>(-12) / SFixedInt<8>(3) == SFixedInt<8>(-4));
static_assert(SFixedInt<8>(-12) / SFixedInt<8>(5) == SFixedInt<8>(-2));
static_assert(SFixedInt<8>(-12) / SFixedInt<8>(-5) == SFixedInt<8>(2));
static_assert(SFixedInt<8>::MinValue / SFixedInt<8>(-1) == SFixedInt<8>::MinValue); // overflow, should remain the same

// modulo
static_assert(UFixedInt<8>(12) % UFixedInt<8>(5) == UFixedInt<8>(2));
static_assert(UFixedInt<8>(12) % UFixedInt<8>(3) == UFixedInt<8>(0));
static_assert(SFixedInt<8>(-12) % SFixedInt<8>(5) == SFixedInt<8>(-2));
static_assert(SFixedInt<8>(-12) % SFixedInt<8>(-5) == SFixedInt<8>(-2));

// bit shifts
static_assert(UFixedInt<8>(1) << 3 == UFixedInt<8>(8));
static_assert(UFixedInt<8>(8) >> 3 == UFixedInt<8>(1));
static_assert(UFixedInt<5>(16) << 1 == UFixedInt<5>::MinValue); // overflow, should wrap around
static_assert(SFixedInt<5>(-3) >> 1 == SFixedInt<5>(-2)); // truncates away from 0
static_assert(SFixedInt<5>(3) >> 1 == SFixedInt<5>(1));

// to string
static_assert(UFixedInt<8>(42).ToString() == "42");
static_assert(UFixedInt<128>(42).ToString() == "42");
static_assert((UFixedInt<128>(1'000'000'000'000) * 1'000'000'000'000).ToString() == "1" + std::string(24, '0'));
static_assert(SFixedInt<8>(-42).ToString() == "-42");
static_assert(UFixedInt<8>(0).ToString() == "0");
static_assert(SFixedInt<8>::MinValue.ToString() == "-128");
static_assert(UFixedInt<2>(3).ToString() == "3");