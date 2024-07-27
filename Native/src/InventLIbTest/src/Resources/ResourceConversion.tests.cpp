#include "InventLib/Resources/ResourceConversion.h"

#include "CommonTest.h"

#include "InventLib/Character/Life.h"

namespace Invent {
    struct ResourceConversionTest : public ::testing::Test {
        ResourceCollection target{};
        ResourceName c1 = ResourceName::Primary;
        ResourceName c2 = ResourceName::Knowledge;
        ResourceName p1 = ResourceName::Money;
        ResourceName p2 = ResourceName::Followers;

        ResourceConversion getSingle() {
            return ResourceConversionBuilder("Test").WithCost(c1, 10).WithProduct(p1, 5).Build();
        }

        ResourceConversion getDouble() {
            return ResourceConversionBuilder("Test")
                .WithCost(c1, 10)
                .WithCost(c2, 5)
                .WithProduct(p1, 5)
                .WithProduct(p2, 2)
                .Build();
        }
    };

    TEST_F(ResourceConversionTest, Convert_MissingResources_DoesNothing) {
        getSingle().Convert(target);

        ASSERT_EQ(0, target[p1].Current);
        ASSERT_EQ(0, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert_WithResources_ReducesByCost) {
        target[c1].Current = 15;

        getSingle().Convert(target);

        ASSERT_EQ(5, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert_WithResources_AddsProducts) {
        target[c1].Current = 15;
        getSingle().Convert(target);

        ASSERT_EQ(5, target[p1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With3xResources_ReducesBy3x) {
        target[c1].Current = 30;
        getSingle().Convert(target, 3);

        ASSERT_EQ(0, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With3xResources_Adds3xProducts) {
        target[c1].Current = 30;
        getSingle().Convert(target, 3);

        ASSERT_EQ(15, target[p1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With2xResources_ReducesBy2x) {
        target[c1].Current = 25;
        getSingle().Convert(target, 3);

        ASSERT_EQ(5, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With2xResources_Adds2xProducts) {
        target[c1].Current = 25;
        getSingle().Convert(target, 3);

        ASSERT_EQ(10, target[p1].Current);
    }

    TEST_F(ResourceConversionTest, GetMaxCount_WithCapacityFor2_Returns2) {
        target[c1].Current = 100;
        target[p1].Current = target[p1].Capacity - 10;
        ASSERT_EQ(2, getSingle().GetMaxCount(target));
    }

    TEST_F(ResourceConversionTest, GetMaxCount_WithCoprime_AllowsPurchasingToFull) { 
        auto converter = getSingle();
        target[c1].Current = 100;
        target[p1].Current = 8;
        target[p1].Capacity = 10;
        ASSERT_EQ(1, converter.GetMaxCount(target));
    }

    TEST_F(ResourceConversionTest, ConvertMultiple_MissingOneResource_DoesNothing) {
        target[c1].Current = 10;
        getDouble().Convert(target);

        ASSERT_EQ(0, target[p1].Current);
        ASSERT_EQ(0, target[p2].Current);
        ASSERT_EQ(10, target[c1].Current);
        ASSERT_EQ(0, target[c2].Current);
    }

    TEST_F(ResourceConversionTest, ConvertMultiple_WithResources_RemovesCosts) {
        target[c1].Current = 15;
        target[c2].Current = 10;
        getDouble().Convert(target);

        ASSERT_EQ(5, target[c1].Current);
        ASSERT_EQ(5, target[c2].Current);
    }

    TEST_F(ResourceConversionTest, ConvertMultiple_WithResources_AddsProducts) {
        target[c1].Current = 15;
        target[c2].Current = 10;
        getDouble().Convert(target);

        ASSERT_EQ(5, target[p1].Current);
        ASSERT_EQ(2, target[p2].Current);
    }
} // namespace Invent