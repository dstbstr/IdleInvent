#include "Math/MathEval.h"
#include "Math/MathTokenizer.h"
#include "Math/MathParser.h"

#include <map>

namespace {
}

namespace Math {
	f64 Eval(std::string_view expr, f64 x) {
		auto postfix = ToPostfix(Normalize(Tokenize(expr)));
		std::vector<f64> stack;
		stack.reserve(postfix.size());

		auto pop = [&]{
			if (stack.empty()) throw std::invalid_argument("Stack underflow");
			auto val = stack.back();
			stack.pop_back();
			return val;
		};

		for(const auto& token : postfix) {
			switch(token.Type) {
				using enum ExpressionTokenType;
				case Number:
					stack.push_back(std::stod(std::string(token.Text)));
					break;
				case Variable:
					if (token.Text != "x") throw std::invalid_argument("Unknown variable: " + std::string(token.Text));
					stack.push_back(x);
					break;

				case UnaryOperator: {
					auto val = pop();
					if(token.Text == "-") val = -val;
					else if (token.Text != "+") throw std::invalid_argument("Unknown unary operator: " + std::string(token.Text));
					stack.push_back(val);
					break;
				}
				case BinaryOperator: {
					auto right = pop();
					auto left = pop();
					f64 value = 0.0;
					if (token.Text == "+") value = left + right;
					else if (token.Text == "-") value = left - right;
					else if (token.Text == "*") value = left * right;
					else if (token.Text == "/") {
						if (right == 0.0) throw std::invalid_argument("Division by zero");
						value = left / right;
					}
					else if (token.Text == "^") value = std::pow(left, right);
					else throw std::invalid_argument("Unknown binary operator: " + std::string(token.Text));
					stack.push_back(value);
					break;
				}
				case Function: {
					auto val = pop();
					if (token.Text == "sin") val = std::sin(val);
					else if (token.Text == "cos") val = std::cos(val);
					else if (token.Text == "tan") val = std::tan(val);
					else if (token.Text == "log") {
						if (val <= 0.0) throw std::invalid_argument("Logarithm of non-positive number");
						val = std::log(val);
					}
					else if (token.Text == "sqrt") val = std::sqrt(val);
					else throw std::invalid_argument("Unknown function: " + std::string(token.Text));
					stack.push_back(val);
					break;
				}
				default: throw std::invalid_argument("Unexpected token type in postfix expression");
			}

			if (!std::isfinite(stack.back())) throw std::invalid_argument("Non-finite result during evaluation");
		}

		if (stack.size() != 1) throw std::invalid_argument("Invalid expression: stack size not equal to 1 after evaluation");
		return stack.back();
	}

	std::expected<f64, std::string> TryEval(std::string_view expr, f64 x) {
		try {
			return Eval(expr, x);
		} catch (const std::logic_error& err) {
			return std::unexpected(std::string(err.what()));
		}
	}
}