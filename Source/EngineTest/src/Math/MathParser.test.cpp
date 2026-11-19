#include "Math/MathParser.h"

namespace Math {
	using enum ExpressionTokenType;

	constexpr bool AssertTokens(std::string_view expr, auto... expected) {
		auto actual = Normalize(Tokenize(expr));
		return actual == std::vector<ExpressionToken>{ expected...};
	}

	static_assert(AssertTokens("3 + 4", 
		ExpressionToken{ Number, "3"}, 
		ExpressionToken{ BinaryOperator, "+"}, 
		ExpressionToken{ Number, "4"}
	));

	static_assert(AssertTokens("-3",
		ExpressionToken{ UnaryOperator, "-" }, 
		ExpressionToken{ Number, "3" }
	));

	static_assert(AssertTokens("sin(-x)",
		ExpressionToken{ Function, "sin" },
		ExpressionToken{ OpenParen, "(" },
		ExpressionToken{ UnaryOperator, "-" },
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ CloseParen, ")" }
	));

	static_assert(AssertTokens("-sin(x)",
		ExpressionToken{ UnaryOperator, "-" },
		ExpressionToken{ Function, "sin" },
		ExpressionToken{ OpenParen, "(" },
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ CloseParen, ")" }
	));

	static_assert(AssertTokens("3x",
		ExpressionToken{ Number, "3" },
		ExpressionToken{ BinaryOperator, "*" },
		ExpressionToken{ Variable, "x" }
	));

	static_assert(AssertTokens("3sin(x)",
		ExpressionToken{ Number, "3" },
		ExpressionToken{ BinaryOperator, "*" },
		ExpressionToken{ Function, "sin" },
		ExpressionToken{ OpenParen, "(" },
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ CloseParen, ")" }
	));

	static_assert(AssertTokens("3--x",
		ExpressionToken{ Number, "3"},
		ExpressionToken{ BinaryOperator, "-"},
		ExpressionToken{ UnaryOperator, "-"},
		ExpressionToken{ Variable, "x"}
	));


	constexpr bool AssertPostfix(std::string_view expr, auto... expected) {
		auto actual = ToPostfix(Normalize(Tokenize(expr)));
		return actual == std::vector<ExpressionToken>{ expected... };
	}

	static_assert(AssertPostfix("3 + 4", 
		ExpressionToken{Number, "3"},
		ExpressionToken{Number, "4"},
		ExpressionToken{BinaryOperator, "+"}
	));

	static_assert(AssertPostfix("1 + 2 * 3",
		ExpressionToken{ Number, "1" },
		ExpressionToken{ Number, "2" },
		ExpressionToken{ Number, "3" },
		ExpressionToken{ BinaryOperator, "*" },
		ExpressionToken{ BinaryOperator, "+" }
	));

	static_assert(AssertPostfix("(1 + 2) * 3",
		ExpressionToken{ Number, "1" },
		ExpressionToken{ Number, "2" },
		ExpressionToken{ BinaryOperator, "+" },
		ExpressionToken{ Number, "3" },
		ExpressionToken{ BinaryOperator, "*" }
	));

	static_assert(AssertPostfix("8 - 3 - 1",
		ExpressionToken{ Number, "8" },
		ExpressionToken{ Number, "3" },
		ExpressionToken{ BinaryOperator, "-" },
		ExpressionToken{ Number, "1" },
		ExpressionToken{ BinaryOperator, "-" }
	));

	static_assert(AssertPostfix("2 ^ 3 ^ 4",
		ExpressionToken{Number, "2"},
		ExpressionToken{ Number, "3" },
		ExpressionToken{ Number, "4" },
		ExpressionToken{ BinaryOperator, "^" },
		ExpressionToken{ BinaryOperator, "^" }
	));

	static_assert(AssertPostfix("-x^2",
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ Number, "2" },
		ExpressionToken{ BinaryOperator, "^" },
		ExpressionToken{ UnaryOperator, "-" }
	));

	static_assert(AssertPostfix("(-x)^2",
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ UnaryOperator, "-" },
		ExpressionToken{ Number, "2" },
		ExpressionToken{ BinaryOperator, "^" }
	));

	static_assert(AssertPostfix("2^-x",
		ExpressionToken{ Number, "2" },
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ UnaryOperator, "-" },
		ExpressionToken{ BinaryOperator, "^" }
	));

	static_assert(AssertPostfix("3--x",
		ExpressionToken{Number, "3"},
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ UnaryOperator, "-" },
		ExpressionToken{ BinaryOperator, "-" }
	));

	static_assert(AssertPostfix("3sin(x)",
		ExpressionToken{ Number, "3" },
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ Function, "sin" },
		ExpressionToken{ BinaryOperator, "*" }
	));

	static_assert(AssertPostfix("sqrt(sin(x)+4)",
		ExpressionToken{ Variable, "x" },
		ExpressionToken{ Function, "sin" },
		ExpressionToken{ Number, "4" },
		ExpressionToken{ BinaryOperator, "+" },
		ExpressionToken{ Function, "sqrt" }
	));

	TEST(MathParser, ToPostFix_UnmatchedCloseParen_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("3+4)"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_UnmatchedOpenParen_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("(3+4"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_ReversedParens_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize(")("))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_OperatorAtStart_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("*3+4"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_OperatorAtEnd_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("3+4+"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_EmptyParens_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("()"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_NumbersWithoutOperator_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("3 4"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_OperatorsWithoutOperands_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("3 + * 4"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_FunctionWithoutOperand_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("sin()"))), std::invalid_argument);
	}
	TEST(MathParser, ToPostFix_FunctionWithoutOpenParen_Throws) {
		ASSERT_THROW(ToPostfix(Normalize(Tokenize("sin 3"))), std::invalid_argument);
	}
	
}