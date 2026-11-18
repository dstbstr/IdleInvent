#include "Math/MathTokenizer.h"

#include <vector>

namespace Math {
	constexpr bool IsToken(std::string_view expr, Token expected) {
		auto tokens = Tokenize(expr);
		return tokens.size() == 1 && tokens.at(0) == expected;
	}

	static_assert(IsToken("42", Token{ TokenType::Number, "42" }));
	static_assert(IsToken("3.14", Token{ TokenType::Number, "3.14" }));
	static_assert(IsToken(".35", Token{ TokenType::Number, ".35" }));
	static_assert(IsToken("0.35", Token{ TokenType::Number, "0.35" }));

	static_assert(IsToken("x", Token{ TokenType::Identifier, "x" }));
	static_assert(IsToken("sin", Token{ TokenType::Identifier, "sin" }));
	static_assert(IsToken("tan_2", Token{ TokenType::Identifier, "tan_2" }));
	static_assert(IsToken("_val", Token{ TokenType::Identifier, "_val" }));

	static_assert(IsToken("+", Token{ TokenType::Operator, "+" }));
	static_assert(IsToken("-", Token{ TokenType::Operator, "-" }));
	static_assert(IsToken("*", Token{ TokenType::Operator, "*" }));
	static_assert(IsToken("/", Token{ TokenType::Operator, "/" }));
	static_assert(IsToken("^", Token{ TokenType::Operator, "^" }));

	static_assert(IsToken("(", Token{ TokenType::OpenParen, "(" }));
	static_assert(IsToken(")", Token{ TokenType::CloseParen, ")" }));

	static_assert(Tokenize("3x").size() == 2);
	static_assert(Tokenize("3(x)").size() == 4);
	static_assert(Tokenize("-3").size() == 2);
	static_assert(Tokenize(" \t\r\n").size() == 0);

	// ensure tokenizer respects view length
	static_assert(Tokenize(std::string_view("123+456", 3)).size() == 1);

	TEST(TokenizeTest, Tokenize_WithUnknownOperator_Throws) {
		EXPECT_THROW(Tokenize("3$"), std::invalid_argument);
	}
	TEST(TokenizeTest, Tokenize_WithMultipleDots_Throws) {
		ASSERT_THROW(Tokenize("3.14.15"), std::invalid_argument);
	}
}