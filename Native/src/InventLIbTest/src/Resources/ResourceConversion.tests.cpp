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

        ResourceConversion singleConversion{};
        ResourceConversion doubleConversion{};

        void SetUp() override {
            singleConversion = ResourceConversionBuilder("Test").WithCost(c1, 10).WithProduct(p1, 5).Build();

            doubleConversion = ResourceConversionBuilder("Test")
                                   .WithCost(c1, 10)
                                   .WithCost(c2, 5)
                                   .WithProduct(p1, 5)
                                   .WithProduct(p2, 2)
                                   .Build();
        }
    };

    TEST_F(ResourceConversionTest, Convert_MissingResources_DoesNothing) {
        singleConversion.Convert(target);

        ASSERT_EQ(0, target[p1].Current);
        ASSERT_EQ(0, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert_WithResources_ReducesByCost) {
        target[c1].Current = 15;

        singleConversion.Convert(target);

        ASSERT_EQ(5, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert_WithResources_AddsProducts) {
        target[c1].Current = 15;
        singleConversion.Convert(target);

        ASSERT_EQ(5, target[p1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With3xResources_ReducesBy3x) {
        target[c1].Current = 30;
        singleConversion.Convert(target, 3);

        ASSERT_EQ(0, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With3xResources_Adds3xProducts) {
        target[c1].Current = 30;
        singleConversion.Convert(target, 3);

        ASSERT_EQ(15, target[p1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With2xResources_ReducesBy2x) {
        target[c1].Current = 25;
        singleConversion.Convert(target, 3);

        ASSERT_EQ(5, target[c1].Current);
    }

    TEST_F(ResourceConversionTest, Convert3_With2xResources_Adds2xProducts) {
		target[c1].Current = 25;
		singleConversion.Convert(target, 3);

		ASSERT_EQ(10, target[p1].Current);
	}

    TEST_F(ResourceConversionTest, ConvertMultiple_MissingOneResource_DoesNothing) {
        target[c1].Current = 10;
        doubleConversion.Convert(target);

        ASSERT_EQ(0, target[p1].Current);
        ASSERT_EQ(0, target[p2].Current);
        ASSERT_EQ(10, target[c1].Current);
        ASSERT_EQ(0, target[c2].Current);
    }

    TEST_F(ResourceConversionTest, ConvertMultiple_WithResources_RemovesCosts) {
        target[c1].Current = 15;
		target[c2].Current = 10;
		doubleConversion.Convert(target);

		ASSERT_EQ(5, target[c1].Current);
		ASSERT_EQ(5, target[c2].Current);
    }

    TEST_F(ResourceConversionTest, ConvertMultiple_WithResources_AddsProducts) {
        target[c1].Current = 15;
        target[c2].Current = 10;
        doubleConversion.Convert(target);

        ASSERT_EQ(5, target[p1].Current);
        ASSERT_EQ(2, target[p2].Current);
    }
} // namespace Invent