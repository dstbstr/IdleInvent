#include "CommonTest.h"

#include "Utilities/TypeUtils.h"

TEST(TypeUtilsTest, Id_ForDifferentTypes_Differs) {
	ASSERT_NE(Type::Id<int>(), Type::Id<float>());
}
TEST(TypeUtilsTest, Id_ForSameType_Matches) {
	ASSERT_EQ(Type::Id<int>(), Type::Id<int>());
}
struct Base {};
struct Derived : Base {};
TEST(TypeUtilsTest, Id_ForDerivedAndBase_Differs) {
	ASSERT_NE(Type::Id<Base>(), Type::Id<Derived>()); 
}

TEST(TypeUtilsTest, Name_ForUdt_HasTypeName) {
	auto name = Type::Name<Base>();
    ASSERT_THAT(name, HasSubstr("Base"));
}

template<typename T>
struct Foo {};

TEST(TypeUtilsTest, Name_WithTemplateType_HasTemplate) {
	auto name = Type::Name<Foo<int>>();
	ASSERT_THAT(name, HasSubstr("Foo"));
    ASSERT_THAT(name, HasSubstr("int"));
}