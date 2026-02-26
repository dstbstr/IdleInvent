#include "CommonTest.h"
#include "Utilities/Handle.h"

TEST(HandleTest, DefaultConstructor_SetsInvalidValue) {
	Handle handle;
	ASSERT_FALSE(handle);
	ASSERT_FALSE(handle.IsValid());
}

TEST(HandleTest, ConstructorWithValue_SetsValue) {
	Handle handle(42);
	ASSERT_TRUE(handle);
	ASSERT_TRUE(handle.IsValid());
	ASSERT_EQ(handle.Value, 42u);
}

TEST(HandleTest, Equality_IsByValue) {
	Handle h1(42);
    Handle h2(42);
    Handle h3(43);
    ASSERT_EQ(h1, h2);
    ASSERT_NE(h1, h3);
}


TEST(ScopedHandleTest, Destructor_InvokesCallback) {
    bool result = false;
    {
        ScopedHandle h(42, [&result]() { result = true; });
	}
    ASSERT_TRUE(result);
}