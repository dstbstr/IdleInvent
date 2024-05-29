#include "CommonTest.h"
#include "Core/DesignPatterns/ServiceLocator.h"

struct ServiceLocatorTest : public testing::Test {
    ServiceLocator& loc = ServiceLocator::Get();

    void TearDown() override {
        loc.Reset<int>();
    }
};

TEST_F(ServiceLocatorTest, IsSet_WithEmptyLocator_ReturnsFalse) {
    ASSERT_FALSE(loc.IsSet<int>());
}

TEST_F(ServiceLocatorTest, Get_WithEmptyLocator_ReturnsNullopt) {
    ASSERT_EQ(nullptr, loc.Get<int>());
}

TEST_F(ServiceLocatorTest, IsSet_AfterSetting_ReturnsTrue) {
    loc.Set<int>();

    ASSERT_TRUE(loc.IsSet<int>());
}

TEST_F(ServiceLocatorTest, Get_AfterSetting_ReturnsSetValue) {
    loc.Set<int>(42);

    auto* result = loc.Get<int>();
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(42, *result);
}

TEST_F(ServiceLocatorTest, Get_AfterDifferentQueryAndSet_ReturnsSetValue) {
    loc.IsSet<long>();
    loc.Set<int>(42);

    auto result = loc.Get<int>();
    ASSERT_NE(nullptr, result);
    ASSERT_EQ(42, *result);
    ASSERT_FALSE(loc.IsSet<long>());
}

TEST_F(ServiceLocatorTest, ResetAll_RemovesEntries_InReverse) {
    static std::vector<std::string> order;
    struct A {
        ~A() {
            order.push_back("A");
        }
    };
    struct B {
        ~B() {
			order.push_back("B");
		}
	};

    loc.Set<A>();
    loc.Set<B>();

	loc.ResetAll();

    ASSERT_EQ(2, order.size());
    ASSERT_EQ("B", order[0]);
    ASSERT_EQ("A", order[1]);
}

struct IFoo {
    virtual ~IFoo() = default;
    virtual int Get() const = 0;
};

struct FooImpl : public IFoo {
    FooImpl(int val) : val(val) {}
    int Get() const override {
		return val;
	}

private:
	int val;
};

TEST_F(ServiceLocatorTest, Get_WithInterface_ReturnsSetValue) {
    loc.SetThisAsThat<FooImpl, IFoo>(42);

	auto result = loc.Get<IFoo>();
	ASSERT_NE(nullptr, result);
	ASSERT_EQ(42, result->Get());
}