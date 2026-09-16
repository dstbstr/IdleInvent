#include "Math/BigInt.h"

static_assert(sizeof(BigInt) == sizeof(u32) * 2);

// construction
// 1e12'000
static_assert(BigInt::Pow10(12'000) > BigInt(std::numeric_limits<u64>::max()));
// 1.234e12'000
static_assert(BigInt::FromScientific(1'234, 11'997) > BigInt(std::numeric_limits<u64>::max()));

// equality
static_assert(BigInt(42u) == BigInt(42u));
static_assert(BigInt(42ull) == BigInt(42u));
static_assert(BigInt(0) == BigInt(0u));
static_assert(BigInt::MaxValue == BigInt::MaxValue);
static_assert(BigInt::MinValue == BigInt::MinValue);
static_assert(BigInt::MinValue < BigInt::MaxValue);

// positive comparisons
static_assert(BigInt(123u) > BigInt(12u));
static_assert(BigInt(99u) < BigInt(100u));
static_assert(BigInt(120u) < BigInt(123u));
static_assert(BigInt(119u) < BigInt(120u));
static_assert(BigInt(100u) < BigInt(101u));
static_assert(BigInt(0u) < BigInt(1u));

// negative comparisons
static_assert(BigInt(-42) == BigInt(-42));
static_assert(BigInt(-42) != BigInt(42));
static_assert(BigInt(-100) < BigInt(-2));
static_assert(BigInt(-120) < BigInt(-119));
static_assert(BigInt(-1) < BigInt(0));

// unary minus
static_assert(BigInt(-42) == -BigInt(42));
static_assert(BigInt(0) == -BigInt(0));

// operator +
static_assert(BigInt(24) + BigInt(24) == BigInt(48));
static_assert(BigInt(42) + 24 == BigInt(66));
static_assert(BigInt(0) + 32 == BigInt(32));
static_assert(BigInt(32) + 0 == BigInt(32));
static_assert(BigInt(-7) + 3 == BigInt(-4));
static_assert(BigInt(-3) + 7 == BigInt(4));
static_assert(BigInt(7) + -3 == BigInt(4));
static_assert(BigInt(-7) + 7 == BigInt(0));

// operator *
static_assert(BigInt(12) * 30 == BigInt(360));
static_assert(BigInt(-7) * 3 == BigInt(-21));
static_assert(BigInt(-7) * -3 == BigInt(21));
static_assert(BigInt::MaxValue * 0 == BigInt(0));
static_assert(BigInt::MaxValue * 10 == BigInt::MaxValue);

// operator * with float
static_assert(BigInt(12) * 0.5f == BigInt(6));
static_assert(BigInt(12) * 1.5f == BigInt(18));
static_assert(BigInt(7) * 0.5f == BigInt(3));

// operator /
static_assert(BigInt(12) / 2 == BigInt(6));
static_assert(BigInt(12) / 5 == BigInt(2));
static_assert(BigInt(-12) / 2 == BigInt(-6));
static_assert(BigInt(-12) / -2 == BigInt(6));

// Pow
static_assert(BigInt(2).Pow(10) == BigInt(1'024));
static_assert(BigInt(-3).Pow(3) == BigInt(-27));
static_assert(BigInt(-3).Pow(4) == BigInt(81));
static_assert(BigInt(42).Pow(0) == BigInt(1));

// Human readable
static_assert(BigInt(0).ToHumanReadable() == "0");
static_assert(BigInt(1'234'567).ToHumanReadable() == "1.23M");
static_assert(BigInt(123'456).ToHumanReadable() == "123.45K");
static_assert(BigInt(10'000).ToHumanReadable(2) == "10.00K");
static_assert(BigInt(-1'234).ToHumanReadable(2) == "-1.23K");

static_assert(BigInt(999).ToHumanReadable() == "999.00");
static_assert(BigInt(1000).ToHumanReadable() == "1.00K");
static_assert(BigInt::Pow10(93).ToHumanReadable() == "1.00Trd");
static_assert(BigInt::Pow10(95).ToHumanReadable() == "100.00Trd");
static_assert(BigInt::Pow10(96).ToHumanReadable() == std::nullopt);

static_assert(BigInt(999).ToHumanReadable(0) == "999");

// To Scientific
static_assert(BigInt(1'234'567).ToScientific() == "1.23e6");
static_assert(BigInt(123'456).ToScientific() == "1.23e5");
static_assert(BigInt(10'000).ToScientific() == "1.00e4");
static_assert(BigInt(-1'234).ToScientific() == "-1.23e3");