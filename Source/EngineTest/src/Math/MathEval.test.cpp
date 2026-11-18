#include "Math/MathEval.h"

namespace Math {
	TEST(MathEval, Eval_SimpleNumber_ReturnsNumber) {
		ASSERT_EQ(Eval("42"), 42.0);
	}
	TEST(MathEval, Eval_Addition_ReturnsSum) {
		ASSERT_EQ(Eval("1 + 2"), 3.0);
	}
	TEST(MathEval, Eval_Subtraction_ReturnsDifference) {
		ASSERT_EQ(Eval("5 - 3"), 2.0);
	}
	TEST(MathEval, Eval_Multiplication_ReturnsProduct) {
		ASSERT_EQ(Eval("4 * 3"), 12.0);
	}
	TEST(MathEval, Eval_Division_ReturnsQuotient) {
		ASSERT_EQ(Eval("10 / 2"), 5.0);
	}
	TEST(MathEval, Eval_Exponent_ReturnsExponent) {
		ASSERT_EQ(Eval("2 ^ 3"), 8.0);
	}
	TEST(MathEval, Eval_Parentheses_ReturnsCorrectResult) {
		ASSERT_EQ(Eval("2 * (3 + 4)"), 14.0);
	}
	TEST(MathEval, Eval_ComplexExpression_ReturnsCorrectResult) {
		ASSERT_EQ(Eval("1 + 2 * 3"), 7.0);
		ASSERT_EQ(Eval("(1 + 2) * 3"), 9.0);
		ASSERT_EQ(Eval("10 / (5 - 3)"), 5.0);
	}
	TEST(MathEval, Eval_WhitespaceHandling_ReturnsCorrectResult) {
		ASSERT_EQ(Eval("  1 + 2 "), 3.0);
		ASSERT_EQ(Eval("3*4"), 12.0);
		ASSERT_EQ(Eval(" 5 / 2 "), 2.5);
	}
	TEST(MathEval, Eval_NestedParentheses_ReturnsCorrectResult) {
		ASSERT_EQ(Eval("2 * (3 + (4 - 1))"), 12.0);
		ASSERT_EQ(Eval("(1 + (2 * 3)) - 4"), 3.0);
	}

	TEST(MathEval, Eval_WithVariableX_ReturnsCorrectResult) {
		ASSERT_EQ(Eval("x + 2", 3.0), 5.0);
		ASSERT_EQ(Eval("2 * x - 1", 4.0), 7.0);
		ASSERT_EQ(Eval("(x + 1) * (x - 1)", 5.0), 24.0);
	}
}
