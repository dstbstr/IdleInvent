#include "CommonTest.h"

#include "Resources/Resource.h"

struct ResourceTest : testing::Test {
    Resource resource{};
    void SetUp() override {
        // Reset resource to default state before each test
        resource.Current = 0;
        resource.BaseCapacity = 100;
        resource.Capacity = 100;
        resource.Name = "TestResource";
    }
};

TEST_F(ResourceTest, Clamp_BelowZero_ClampsToZero) {
    resource.Current = -10;
    resource.Clamp();
    ASSERT_EQ(resource.Current, 0);
}
TEST_F(ResourceTest, Clamp_AboveCapacity_ClampsToCapacity) {
    resource.Current = resource.Capacity + 150;
    resource.Clamp();
    ASSERT_EQ(resource.Current, resource.Capacity);
}
TEST_F(ResourceTest, Clamp_BetweenZeroAndCapacity_DoesNotChange) {
    resource.Current = resource.Capacity / 2;
    resource.Clamp();
    ASSERT_EQ(resource.Current, resource.Capacity / 2);
}
TEST_F(ResourceTest, AddCapacityModifier_AdditiveModifier_IncreasesCapacity) {
    resource.AddCapacityModifier({50, 1.0f});
    ASSERT_EQ(resource.Capacity, 150);
}

TEST_F(ResourceTest, AddCapacityModifier_MultiplicativeModifier_ScalesCapacity) {
    resource.AddCapacityModifier({0, 2.0f});
    ASSERT_EQ(resource.Capacity, 200);
}

TEST_F(ResourceTest, AddCapacityModifier_CombinedModifier_AppliesCorrectly) {
    resource.AddCapacityModifier({50, 2.0f});
    ASSERT_EQ(resource.Capacity, 300); // (100 + 50) * 2.0
}

TEST_F(ResourceTest, RemoveCapacityModifier_ExistingModifier_RestoresCapacity) {
    Modifier mod{50, 1.0f};
    resource.AddCapacityModifier(mod);
    resource.RemoveCapacityModifier(mod);
    ASSERT_EQ(resource.Capacity, resource.BaseCapacity);
}

TEST_F(ResourceTest, RemoveCapacityModifier_NonExistentModifier_NoChange) {
    resource.AddCapacityModifier({50, 1.0f});
    resource.RemoveCapacityModifier({25, 1.0f});
    ASSERT_EQ(resource.Capacity, 150);
}

TEST_F(ResourceTest, MultipleModifiers_StackCorrectly) {
    resource.AddCapacityModifier({10, 1.0f});
    resource.AddCapacityModifier({20, 2.0f});
    // Combined: Add=30, Mul=2.0 -> (100 + 30) * 2.0 = 260
    ASSERT_EQ(resource.Capacity, 260);
}

// ===== ResourceCollection Tests =====

enum struct TestResource : u8 { Sticks, Stones, Bones, COUNT };
std::string ToString(TestResource resource) {
    switch(resource) {
    case TestResource::Sticks: return "Sticks";
    case TestResource::Stones: return "Stones";
    case TestResource::Bones: return "Bones";
    default: return "Unknown";
    }
}

TEST(ResourceCollectionTest, DefaultConstruction_IsEmpty) {
    ResourceCollection collection{};
    ASSERT_TRUE(collection.IsEmpty());
}

TEST(ResourceCollectionTest, AddResources_CreatesEntries) {
    ResourceCollection collection{};
    collection.AddResources({0, 1, 2});
    ASSERT_EQ(collection[0].Current, 0);
    ASSERT_EQ(collection[1].Current, 0);
    ASSERT_EQ(collection[2].Current, 0);
}

TEST(ResourceCollectionTest, IsEmpty_WithNonZeroResource_ReturnsFalse) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, 1}});

    ASSERT_FALSE(rc.IsEmpty());
}

TEST(ResourceCollectionTest, Clamp_ClampsAllResources) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, -10}, {Stones, 250}});
    rc.Clamp();
    ASSERT_EQ(rc[Sticks].Current, 0);
    ASSERT_EQ(rc[Stones].Current, rc[Stones].Capacity);
}


TEST(ResourceCollectionTest, CanAfford_WithEnoughResources_ReturnsTrue) {
    auto avail = CreateRc<TestResource>({{TestResource::Sticks, 10}, {TestResource::Stones, 20}});
    auto cost = CreateRc<TestResource>({{TestResource::Sticks, 5}, {TestResource::Stones, 15}});
    ASSERT_TRUE(avail.CanAfford(cost));
    ASSERT_FALSE(cost.CanAfford(avail));
}

TEST(ResourceCollectionTest, CanAfford_WithEqualResources_ReturnsTrue) {
    auto rc = CreateRc<TestResource>({{TestResource::Sticks, 10}, {TestResource::Stones, 20}});
    ASSERT_TRUE(rc.CanAfford(rc));
}

TEST(ResourceCollectionTest, PlusEquals_AddsResources) {
    using enum TestResource;
    auto lhs = CreateRc<TestResource>({{Sticks, 10}});
    auto rhs = CreateRc<TestResource>({{Sticks, 5}});
    lhs += rhs;
    ASSERT_EQ(lhs[Sticks].Current, 15);
}

TEST(ResourceCollectionTest, MinusEquals_SubtractsResources) {
    using enum TestResource;
    auto lhs = CreateRc<TestResource>({{Sticks, 10}});
    auto rhs = CreateRc<TestResource>({{Sticks, 5}});
    lhs -= rhs;
    ASSERT_EQ(lhs[Sticks].Current, 5);
}

TEST(ResourceCollectionTest, MultiplyEquals_ScalesResources) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, 10}});
    rc *= 3ull;

    ASSERT_EQ(rc[Sticks].Current, 30);
}

TEST(ResourceCollectionTest, DivideEquals_DividesResources) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, 10}});
    rc /= 3ull;

    ASSERT_EQ(rc[Sticks].Current, 3);
}


TEST(ResourceCollectionTest, ToCostString_WithAllValues_IncludesAllNames) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, 2}, {Stones, 3}, {Bones, 4}});
    auto str = rc.ToCostString();
    ASSERT_THAT(str, HasSubstr("Sticks"));
    ASSERT_THAT(str, HasSubstr("Stones"));
    ASSERT_THAT(str, HasSubstr("Bones"));
}

TEST(ResourceCollectionTest, ToCostString_MissingValues_ExcludesMissinvValues) {
    using enum TestResource;
    auto rc = CreateRc<TestResource>({{Sticks, 2}, {Stones, 3}});

    auto str = rc.ToCostString();
    ASSERT_THAT(str, HasSubstr("Sticks"));
    ASSERT_THAT(str, HasSubstr("Stones"));
    ASSERT_THAT(str, Not(HasSubstr("Bones")));
}

TEST(ResourceCollectionTest, OperatorEqual_WithAllEqual_ReturnsTrue) {
    using enum TestResource;
    auto a = CreateRc<TestResource>({{Sticks, 10}, {Stones, 20}});
    auto b = CreateRc<TestResource>({{Sticks, 10}, {Stones, 20}});

    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a != b);
}

TEST(ResourceCollectionTest, OperatorEqual_WithOneDifferent_ReturnsFalse) {
    using enum TestResource;
    auto a = CreateRc<TestResource>({{Sticks, 10}, {Stones, 20}});
    auto b = CreateRc<TestResource>({{Sticks, 10}, {Stones, 15}});

    ASSERT_FALSE(a == b);
    ASSERT_TRUE(a != b);
}

