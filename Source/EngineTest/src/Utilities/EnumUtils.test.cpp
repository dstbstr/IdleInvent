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

enum struct EnumWithCount : u8 {
	A, B, C, COUNT 
};

TEST(EnumUtilsTest, GetAllValues_WithoutUnset_GetsAllValues) {
	auto values = Enum::GetAllValues<EnumWithCount>();
	EXPECT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], EnumWithCount::A);
	EXPECT_EQ(values[1], EnumWithCount::B);
	EXPECT_EQ(values[2], EnumWithCount::C);
}

TEST(EnumUtilsTest, GetAllValues_WithUnset_GetsAllValuesExceptUnset) {
	auto values = Enum::GetAllValues<EnumWithUnset>();
	EXPECT_EQ(values.size(), 3);
	EXPECT_EQ(values[0], EnumWithUnset::A);
	EXPECT_EQ(values[1], EnumWithUnset::B);
	EXPECT_EQ(values[2], EnumWithUnset::C);
}