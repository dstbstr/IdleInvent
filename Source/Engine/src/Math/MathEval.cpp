#include "Math/MathEval.h"
#include "Math/MathTokenizer.h"

#include <map>

namespace {
	enum struct Associativity { Left, Right };

	template<typename T>
	struct OperatorInfo {
		size_t Precedence;
		Associativity Assoc = Associativity::Left;
		std::function<T(T, T)> Run;
	};

	template<typename T>
	std::map<std::string_view, OperatorInfo<T>> DefaultOperators = {
		{ "+", OperatorInfo<T>{ 2, Associativity::Left, [](T a, T b) -> T { return a + b; } } },
		{ "-", OperatorInfo<T>{ 2, Associativity::Left, [](T a, T b) -> T { return a - b; } } },
		{ "*", OperatorInfo<T>{ 3, Associativity::Left, [](T a, T b) -> T { return a * b; } } },
		{ "/", OperatorInfo<T>{ 3, Associativity::Left, [](T a, T b) -> T { return a / b; } } },
		{ "^", OperatorInfo<T>{ 4, Associativity::Right, [](T a, T b) -> T { return std::pow(a, b); } } }
	};

	constexpr auto KnownFunctions = { "sin", "cos", "tan", "log", "sqrt" };

	enum struct ExpressionTokenType {
		Number,
		Variable,
		UnaryOperator,
		BinaryOperator,
		Function,
		OpenParen,
		CloseParen
	};

	struct ExpressionToken {
		ExpressionTokenType Type;
		std::string_view Text;
	};

	constexpr std::vector<ExpressionToken> Normalize(std::span<const Math::Token> tokens) {
		using namespace Math;

		std::vector<ExpressionToken> result;
		auto expectOperand = true;

		for (size_t i = 0; i < tokens.size(); ++i) {
			const auto& token = tokens[i];
			if(token.Type == TokenType::Number) {
				result.push_back({ExpressionTokenType::Number, token.Text});
				expectOperand = false;
			}
			else if (token.Type == TokenType::OpenParen) {
				if(!expectOperand) {
					result.push_back({ExpressionTokenType::BinaryOperator, "*"});
				}
				result.push_back({ ExpressionTokenType::OpenParen, token.Text });
				expectOperand = true;
			}
			else if (token.Type == TokenType::CloseParen) {
				result.push_back({ ExpressionTokenType::CloseParen, token.Text });
				expectOperand = false;
			}
			else if (token.Type == TokenType::Operator) {
				if (token.Text == "+" || token.Text == "-") {
					result.push_back({ expectOperand ? ExpressionTokenType::UnaryOperator : ExpressionTokenType::BinaryOperator, token.Text });
				} else {
					result.push_back({ ExpressionTokenType::BinaryOperator, token.Text });
				}
				expectOperand = true;
			}
			else if (token.Type == TokenType::Identifier) {
				bool isFunc = false;
				for (auto func : KnownFunctions) {
					if (token.Text == func) {
						isFunc = true;
						break;
					}
				}

				if (!expectOperand) {
					result.push_back({ ExpressionTokenType::BinaryOperator, "*" });
				}

				if (isFunc) {
					result.push_back({ ExpressionTokenType::Function, token.Text });
					expectOperand = true;
				} else {
					result.push_back({ ExpressionTokenType::Variable, token.Text });
					expectOperand = false;
				}
			}
		}

		return result;
	}

	template<typename T>
	constexpr void ProcessToken(std::string_view token, std::vector<std::string_view>& output, std::vector<std::string_view>& operators, const std::map<std::string_view, OperatorInfo<T>>& opInfo) {
		auto firstOpen = token.find('(');
		auto firstClose = token.find(')');
		auto firstNum = token.find_first_of("0123456789");
		auto firstVar = token.find_first_of("x");
		auto firstSymbol = token.find_first_not_of("0123456789x()");

		std::vector<size_t> positions = { firstOpen, firstClose, firstNum, firstVar, firstSymbol };
		positions.erase(std::remove_if(positions.begin(), positions.end(), [token](size_t pos) { return pos == token.npos; }), positions.end());

		if (positions.empty()) return;
		if (positions.size() > 1) {
			std::sort(positions.begin(), positions.end());
			ProcessToken(token.substr(0, positions[1]), output, operators, opInfo);
			ProcessToken(token.substr(positions[1]), output, operators, opInfo);
			return;
		}

		if (firstOpen != std::string_view::npos) {
			for (size_t i = 0u; i < token.size(); i++) {
				operators.emplace_back("(");
			}
		}
		else if (firstNum != token.npos) output.push_back(token);
		else if (firstClose != token.npos) {
			for (size_t i = 0u; i < token.size(); i++) {
				while (true) {
					auto next = operators.back(); operators.pop_back();
					if (next == "(") break;
					output.push_back(next);
				}
			}
		}
		else if (firstVar != token.npos) output.push_back(token);
		else if (firstSymbol != token.npos) {
			auto o1 = opInfo.at(token);
			auto shouldPop = [&]() {
				if (operators.empty()) return false;
				auto next = operators.back();
				if (next == "(") return false;
				auto o2 = opInfo.at(next);
				if (o1.Precedence != o2.Precedence) return o2.Precedence > o1.Precedence;
				return o1.Assoc == Associativity::Left;
				};
			while (shouldPop()) {
				output.push_back(operators.back());
				operators.pop_back();
			}
			operators.push_back(token);
		}
	}
}

namespace Math {
	f64 Eval(std::string_view expr, f64 x) {
		auto tokens = Tokenize(expr);

		auto tokens = Constexpr::Split(expr, " ");
		std::vector<std::string_view> outputQueue;
		std::vector<std::string_view> operatorStack;

		for (const auto& token : tokens) {
			ProcessToken(token, outputQueue, operatorStack, DefaultOperators<f64>);
		}
		while (!operatorStack.empty()) {
			outputQueue.push_back(operatorStack.back());
			operatorStack.pop_back();
		}

		std::vector<f64> evalStack;
		while (!outputQueue.empty()) {
			auto token = outputQueue.front(); outputQueue.erase(outputQueue.begin());
			if (Constexpr::IsDigit(token[0])) {
				evalStack.emplace_back(std::stod(std::string(token)));
			}
			else if (token == "x") {
				evalStack.emplace_back(x);
			}
			else {
				auto b = evalStack.back(); evalStack.pop_back();
				auto a = evalStack.back(); evalStack.pop_back();
				auto result = DefaultOperators<f64>.at(token).Run(a, b);
				evalStack.push_back(result);
			}
		}

		return evalStack.back();
	}
}