#include "CommonTest.h"

#include "InventLib/Resources/Resource.h"

namespace Invent {
	TEST(ResourceCollection, ResourceCollection_OnConstruction_HasAllResources) {
		ResourceCollection collection;
		for (auto resource : AllResources) {
			ASSERT_EQ(collection[resource], 0);
		}
	}

	TEST(ResourceCollection, Equals_WithSelf_ReturnsTrue) {
		ResourceCollection collection;
		ASSERT_TRUE(collection == collection);
	}

	TEST(ResourceCollection, Equals_WithDifferentResources_ReturnsFalse) {
		ResourceCollection collection1;
		ResourceCollection collection2;
		collection2[ResourceName::Labor] = 1;
		ASSERT_FALSE(collection1 == collection2);
	}

	TEST(ResourceCollection, LessThan_WithSelf_ReturnsFalse) {
		ResourceCollection collection;
		ASSERT_FALSE(collection < collection);
	}

	TEST(ResourceCollection, LessThan_WithLessResources_ReturnsTrue) {
		ResourceCollection collection1;
		ResourceCollection collection2{ {{ResourceName::Labor, 1}, {ResourceName::Influence, 1}, {ResourceName::Knowledge, 1}, {ResourceName::Magic, 1}, {ResourceName::Wealth, 1}} };
		ASSERT_TRUE(collection1 < collection2);
	}

	TEST(ResourceCollection, LessThan_WithMoreResources_ReturnsFalse) {
		ResourceCollection collection1;
		ResourceCollection collection2;
		collection1[ResourceName::Labor] = 1;
		ASSERT_FALSE(collection1 < collection2);
	}

	TEST(ResourceCollection, LessThan_WithLessAndMoreResources_ReturnsFalse) {
		ResourceCollection collection1;
		ResourceCollection collection2;
		collection1[ResourceName::Labor] = 1;
		collection2[ResourceName::Knowledge] = 1;

		// Note: This property means these cannot be sorted
		ASSERT_FALSE(collection1 < collection2);
		ASSERT_FALSE(collection2 < collection1);
	}
}