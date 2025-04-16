#include "Resources/Resource.h"

#include "CommonTest.h"

std::vector<u16> AllResources() { return {1, 2, 3, 4}; }

ResourceCollection MakeCollection(s64 count) {
    ResourceCollection collection{AllResources()};
    for(auto resource: AllResources()) {
        collection[resource].Current = count;
    }
    return collection;
}

TEST(ResourceCollection, ResourceCollection_OnConstruction_HasAllResources) {
    ResourceCollection collection{AllResources()};
    for(auto resource: AllResources()) {
        ASSERT_EQ(collection[resource].Current, 0);
    }
}

TEST(ResourceCollection, Equals_WithSelf_ReturnsTrue) {
    ResourceCollection collection{AllResources()};
    ASSERT_TRUE(collection == collection);
}

TEST(ResourceCollection, Equals_WithDifferentResources_ReturnsFalse) {
    ResourceCollection collection1{AllResources()};
    ResourceCollection collection2{AllResources()};
    collection2[2].Current = 1;
    ASSERT_FALSE(collection1 == collection2);
}

TEST(ResourceCollection, LessThan_WithSelf_ReturnsFalse) {
    ResourceCollection collection{AllResources()};
    ASSERT_FALSE(collection < collection);
}

TEST(ResourceCollection, LessThan_WithLessResources_ReturnsTrue) {
    ResourceCollection collection1{AllResources()};
    auto collection2 = MakeCollection(1);
    ASSERT_TRUE(collection1 < collection2);
}

TEST(ResourceCollection, LessThan_WithMoreResources_ReturnsFalse) {
    ResourceCollection collection1{AllResources()};
    ResourceCollection collection2{AllResources()};
    collection1[2].Current = 1;
    ASSERT_FALSE(collection1 < collection2);
}

TEST(ResourceCollection, LessThan_WithLessAndMoreResources_ReturnsFalse) {
    ResourceCollection collection1{AllResources()};
    ResourceCollection collection2{AllResources()};
    collection1[1].Current = 1;
    collection2[2].Current = 1;

    // Note: This property means these cannot be sorted
    ASSERT_FALSE(collection1 < collection2);
    ASSERT_FALSE(collection2 < collection1);
}
