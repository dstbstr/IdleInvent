#include "CommonTest.h"

#include <format>

TEST(Format, Format_WithNoFormatCharacters_ReturnsOriginalString) {
	ASSERT_EQ(std::format("Hello World"), "Hello World"s);
}

TEST(Format, Format_WithIntFormat_ReturnsFormattedString) {
	ASSERT_EQ(std::format("Hello {}", 42), "Hello 42"s);
}

TEST(Format, Format_WithFloatFormat_ReturnsFormattedString) {
	ASSERT_EQ(std::format("Hello {:.2f}", 42.0f), "Hello 42.00"s);
}

TEST(Format, Format_WithCharStar_FormatsString) {
	ASSERT_EQ(std::format("Hello {}", "World"), "Hello World"s);
}

TEST(Format, Format_WithStdString_FormatsString) {
	ASSERT_EQ(std::format("{}", "test"s), "test"s);
}

TEST(Format, Format_WithStringView_FormatsString) {
	using namespace std::string_view_literals;
	ASSERT_EQ(std::format("Hello {}", "World"sv), "Hello World"s);
}
