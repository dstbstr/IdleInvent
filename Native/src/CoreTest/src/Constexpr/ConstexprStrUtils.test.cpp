#include "CommonTest.h"

#include "Core/Constexpr/ConstexprStrUtils.h"


static_assert(Constexpr::Split("Hello World", " ").size() == 2);
static_assert(Constexpr::Split("Hello World", " ")[0] == "Hello");
static_assert(Constexpr::Split("Hello World", " ")[1] == "World");

static_assert(Constexpr::Split("Hello   World", " ").size() == 2);
static_assert(Constexpr::Split("Hello   World", " ")[0] == "Hello");
static_assert(Constexpr::Split("Hello   World", " ")[1] == "World");

static_assert(Constexpr::Split("Hello World", " ", true).size() == 2);
static_assert(Constexpr::Split("Hello World", " ", true)[0] == "Hello");
static_assert(Constexpr::Split("Hello World", " ", true)[1] == "World");

static_assert(Constexpr::Split("Hello   World", " ", true).size() == 4);
static_assert(Constexpr::Split("Hello   World", " ", true)[0] == "Hello");
static_assert(Constexpr::Split("Hello   World", " ", true)[1] == "");
static_assert(Constexpr::Split("Hello   World", " ", true)[2] == "");
static_assert(Constexpr::Split("Hello   World", " ", true)[3] == "World");

static_assert(Constexpr::HumanReadable(0) == "0");
static_assert(Constexpr::HumanReadable(1000) == "1K");
static_assert(Constexpr::HumanReadable(1'000'000) == "1M");
static_assert(Constexpr::HumanReadable(1'000'000'000) == "1B");
static_assert(Constexpr::HumanReadable(1'000'000'000'000ULL) == "1T");

static_assert(Constexpr::HumanReadable(1234) == "1.23K");
static_assert(Constexpr::HumanReadable(12'345) == "12.34K");
static_assert(Constexpr::HumanReadable(123'456) == "123.45K");

static_assert(Constexpr::HumanReadable(1'234'567) == "1.23M");
static_assert(Constexpr::HumanReadable(12'345'678) == "12.34M");
static_assert(Constexpr::HumanReadable(123'456'789) == "123.45M");

static_assert(Constexpr::TrimStart(" \n Hello") == "Hello");
static_assert(Constexpr::TrimStart("Hello") == "Hello");
static_assert(Constexpr::TrimStart("Hello ") == "Hello ");

static_assert(Constexpr::TrimEnd("Hello \n ") == "Hello");
static_assert(Constexpr::TrimEnd("Hello") == "Hello");
static_assert(Constexpr::TrimEnd(" Hello") == " Hello");

static_assert(Constexpr::Trim(" \n Hello \n ") == "Hello");
static_assert(Constexpr::Trim("Hello") == "Hello");
static_assert(Constexpr::Trim(" Hello ") == "Hello");

static_assert(Constexpr::Trim("Hello World", {'H', 'd'}) == "ello Worl");

static_assert(Constexpr::TimeString(3'400) == "3s");
static_assert(Constexpr::TimeString(1000 * 60) == "1m");
static_assert(Constexpr::TimeString(1000 * 60 * 60) == "1h");
static_assert(Constexpr::TimeString(1000 * 60 * 60 * 24) == "1d");
