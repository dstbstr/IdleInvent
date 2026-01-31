#include "CommonTest.h"

#include "Utilities/EnumUtils.h"
#include <Platform/NumTypes.h>

enum struct EnumWithToString : u8 {
	A, B, C
};
std::string ToString(EnumWithToString) { return ""; }
static_assert(ToStringEnum<EnumWithToString>);

enum struct EnumWithDescribe : u8 {
	A, B, C
};
std::string Describe(EnumWithDescribe) { return ""; }
static_assert(DescribeEnum<EnumWithDescribe>);

enum struct EnumWithUnset : u8 {
	Unset, A, B, C, COUNT
};

static_assert(Enum::Begin<EnumWithUnset>() == EnumWithUnset::A);
static_assert(Enum::End<EnumWithUnset>() == EnumWithUnset::C);
static_assert(Enum::GetAllValues<EnumWithUnset>() == std::vector{EnumWithUnset::A, EnumWithUnset::B, EnumWithUnset::C});

enum struct EnumWithCount : u8 {
	A, B, C, COUNT 
};
static_assert(Enum::Begin<EnumWithCount>() == EnumWithCount::A);
static_assert(Enum::End<EnumWithCount>() == EnumWithCount::C);
static_assert(Enum::GetAllValues<EnumWithCount>() == std::vector{EnumWithCount::A, EnumWithCount::B, EnumWithCount::C});

TEST(EnumUtilsTest, GetAllValues_WithoutUnset_GetsAllValues) {
	auto values = Enum::GetAllValues<EnumWithCount>();
	ASSERT_EQ(values.size(), 3);
	ASSERT_EQ(values[0], EnumWithCount::A);
	ASSERT_EQ(values[1], EnumWithCount::B);
	ASSERT_EQ(values[2], EnumWithCount::C);
}

TEST(EnumUtilsTest, GetAllValues_WithUnset_GetsAllValuesExceptUnset) {
	auto values = Enum::GetAllValues<EnumWithUnset>();
	ASSERT_EQ(values.size(), 3);
	ASSERT_EQ(values[0], EnumWithUnset::A);
	ASSERT_EQ(values[1], EnumWithUnset::B);
	ASSERT_EQ(values[2], EnumWithUnset::C);
}

TEST(EnumUtilsTest, Increment_WithSmallValue_Increments) {
	using enum EnumWithCount;
    auto val = A;
    auto next = Enum::Increment(val);
    ASSERT_EQ(next, B);
}

TEST(EnumUtilsTest, Increment_WithMaxValue_ReturnsMaxValue) {
	using enum EnumWithCount;
	auto val = C;
	auto next = Enum::Increment(val);
	ASSERT_EQ(next, C);
}

TEST(EnumUtilsTest, Decrement_WithLargeValue_Decrements) {
	using enum EnumWithCount;
	auto val = C;
	auto prev = Enum::Decrement(val);
	ASSERT_EQ(prev, B);
}

TEST(EnumUtilsTest, Decrement_WithMinValue_OnEnumWithCount_RemainsMin) {
    using enum EnumWithCount;
    auto val = A;
    auto prev = Enum::Decrement(val);
    ASSERT_EQ(prev, A);
}

TEST(EnumUtilsTest, Decrement_WithMinValue_OnEnumWithUnset_RemainsMin) {
    // Use fully-qualified enumerators to avoid ambiguity with other enums in this TU
    auto val = EnumWithUnset::A;
    auto prev = Enum::Decrement(val);
    ASSERT_EQ(prev, EnumWithUnset::A);
}

enum struct EnumWithFromString : u8 {
	A, B, C, COUNT
};
std::string ToString(EnumWithFromString e) {
	switch(e) {
		using enum EnumWithFromString;
    case A: return "A";
    case B: return "B";
    case C: return "C";
    default: return "";
	}
}
TEST(EnumUtilsTest, FromString_WithMatchingString_ReturnsValue) {
    auto result = Enum::FromString<EnumWithFromString, true>("B");
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result.value(), EnumWithFromString::B);
}
TEST(EnumUtilsTest, FromString_WithNonMatchingString_ReturnsNullopt) {
	auto result = Enum::FromString<EnumWithFromString, true>("D");
	ASSERT_FALSE(result.has_value());
}
TEST(EnumUtilsTest, FromString_CaseInsensitive_MatchesIgnoringCase) {
	auto result = Enum::FromString<EnumWithFromString, false>("c");
	ASSERT_TRUE(result.has_value());
	ASSERT_EQ(result.value(), EnumWithFromString::C);
}
TEST(EnumUtilsTest, FromString_CaseSensitive_DoesNotMatchIgnoringCase) {
	auto result = Enum::FromString<EnumWithFromString, true>("a");
	ASSERT_FALSE(result.has_value());
}