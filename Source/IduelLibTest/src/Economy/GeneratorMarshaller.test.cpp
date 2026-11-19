#include "Iduel/Economy/GeneratorMarshaller.h"

#include <cmath>
#include <sstream>

namespace {
	using namespace Iduel;
	void AssertEqual(const Threshold& lhs, const Threshold& rhs) {
		ASSERT_EQ(lhs.Index, rhs.Index);
		ASSERT_EQ(lhs.IncomeMultiplier, rhs.IncomeMultiplier);
		ASSERT_EQ(lhs.SpeedMultiplier, rhs.SpeedMultiplier);
	}

	void AssertEqual(const Generator& lhs, const Generator& rhs) {
		ASSERT_EQ(lhs.Income.GetExpression(), rhs.Income.GetExpression());
		ASSERT_EQ(lhs.Cost.GetExpression(), rhs.Cost.GetExpression());
		ASSERT_EQ(lhs.Name, rhs.Name);
		ASSERT_EQ(lhs.TimePerIncome, rhs.TimePerIncome);
		ASSERT_EQ(lhs.Thresholds.size(), rhs.Thresholds.size());
		for (size_t i = 0; i < lhs.Thresholds.size(); i++) {
			AssertEqual(lhs.Thresholds[i], rhs.Thresholds[i]);
		}
	}
}

namespace Iduel {
	struct MarshallerTest : public ::testing::Test {
		std::stringstream stream {};
	};	

	TEST_F(MarshallerTest, GeneratorMarshaller_SimpleGenerator_RoundTrips) {
		Generator original{
			.Income = Curve{"2x"},
			.Cost = Curve{"3x"},
			.Name = "Test Generator"
		};
		auto file = GeneratorFile{
			.Name = "TestFile",
			.Generators = { original }
		};
		SerializeGenerators(file, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_EQ(result.Name, file.Name);
		ASSERT_EQ(result.Generators.size(), 1);
		const auto& actual = result.Generators[0];
		AssertEqual(actual, original);
	}

	TEST_F(MarshallerTest, GeneratorMarshaller_GeneratorWithThresholds_RoundTrips) {
		Generator original{
			.Thresholds = {{
				.Index = 0,
				.IncomeMultiplier{ 1.0 },
				.SpeedMultiplier{ 2.0 }
			}, {
				.Index = 1,
				.IncomeMultiplier{ 3.0 },
				.SpeedMultiplier{ 4.0 }
			}}
		};
		auto file = GeneratorFile{
			.Name = "TestFile",
			.Generators = { original }
		};
		SerializeGenerators(file, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_EQ(result.Generators.size(), 1);
		const auto& actual = result.Generators[0];
		AssertEqual(actual, original);
	}

	TEST_F(MarshallerTest, GeneratorMarshaller_MultipleGenerators_RoundTrips) {
		Generator gen1{
			.Income = Curve{ "2x" },
			.Name = "Gen1"
		};
		Generator gen2{
			.Cost = Curve{ "3x" },
			.Name = "Gen2"
		};
		auto file = GeneratorFile{
			.Name = "TestFile",
			.Generators = {gen1, gen2}
		};
		SerializeGenerators(file, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_EQ(result.Generators.size(), file.Generators.size());
		for (size_t i = 0; i < file.Generators.size(); i++) {
			AssertEqual(file.Generators[i], result.Generators[i]);
		}
	}

	TEST_F(MarshallerTest, GeneratorMarshaller_EmptyList_RoundTrips) {
		GeneratorFile original{.Name = "EmptyFile"};
		SerializeGenerators(original, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_TRUE(result.Generators.empty());
	}

	TEST_F(MarshallerTest, GeneratorMarshaller_WithSpaces_RoundTrips) {
		Generator original{
			.Income = Curve{ "2x + 3" },
			.Cost = Curve{ "4x - 5" },
			.Name = "Test Generator"
		};
		auto file = GeneratorFile{
			.Name = "TestFile",
			.Generators = { original }
		};
		SerializeGenerators(file, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_EQ(result.Generators.size(), 1);
		const auto& actual = result.Generators[0];
		AssertEqual(actual, original);
	}

	TEST_F(MarshallerTest, GeneratorMarshaller_WithFloatingPoint_RoundTrips) {
		Generator original{
			.Thresholds = {{
				.Index = 0,
				.IncomeMultiplier = std::nextafter(1.0, 2.0),
				.SpeedMultiplier = std::nextafter(1.1, 2.0)
			}}
		};
		auto file = GeneratorFile{
			.Name = "TestFile",
			.Generators = { original }
		};
		SerializeGenerators(file, stream);
		auto result = DeserializeGenerators(stream);
		ASSERT_EQ(result.Generators.size(), 1);
		const auto& actual = result.Generators[0];
		AssertEqual(actual, original);
	}
}