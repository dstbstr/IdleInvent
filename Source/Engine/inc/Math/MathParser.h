#pragma once

#include "Math/MathTokenizer.h"

#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace Math {
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
		std::string Text;

		constexpr ExpressionToken(ExpressionTokenType type, std::string_view text) : Type(type), Text(text) {}
		constexpr bool operator==(const ExpressionToken& other) const = default;
	};

	struct OperatorInfo {
		int Precedence;
		bool RightAssociative;
	};

	constexpr OperatorInfo GetOperatorInfo(ExpressionToken token) {
		if(token.Type == ExpressionTokenType::BinaryOperator) {
			if (token.Text == "+" || token.Text == "-") {
				return { 1, false };
			}
			else if (token.Text == "*" || token.Text == "/") {
				return { 2, false };
			}
			else if (token.Text == "^") {
				return { 4, true };
			}
		} else if(token.Type == ExpressionTokenType::UnaryOperator) {
			return { 3, true };
		}

		throw std::invalid_argument("Invalid operator token");
	}

	constexpr std::vector<ExpressionToken> Normalize(std::span<const Math::Token> tokens) {
		using namespace Math;

		std::vector<ExpressionToken> result;
		auto expectOperand = true;

		for (size_t i = 0; i < tokens.size(); ++i) {
			const auto& token = tokens[i];
			if (token.Type == TokenType::Number) {
				result.push_back({ ExpressionTokenType::Number, token.Text });
				expectOperand = false;
			}
			else if (token.Type == TokenType::OpenParen) {
				if (!expectOperand) {
					result.push_back({ ExpressionTokenType::BinaryOperator, "*" });
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
				}
				else {
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
				}
				else {
					result.push_back({ ExpressionTokenType::Variable, token.Text });
					expectOperand = false;
				}
			}
		}

		return result;
	}

	constexpr std::vector<ExpressionToken> ToPostfix(std::span<const ExpressionToken> tokens) {
		std::vector<ExpressionToken> outputQueue;
		std::vector<ExpressionToken> operatorStack;

		auto expectOperand = true;
		auto expectOpenParen = false;

		for(const auto& token : tokens) {
			if (expectOpenParen && token.Type != ExpressionTokenType::OpenParen) {
				throw std::invalid_argument("Expected open paren");
			}
			switch(token.Type) {
				using enum ExpressionTokenType;
				case Number: case Variable: {
					if (!expectOperand) throw std::invalid_argument("Unexpected operand");
					expectOperand = false;
					outputQueue.push_back(token); 
					break;
				}
				case OpenParen: case Function: case UnaryOperator: {
					if (!expectOperand) throw std::invalid_argument("Unexpected operand");
					expectOperand = true;
					operatorStack.push_back(token);
					expectOpenParen = token.Type == Function;
					break;
				}
				case CloseParen: {
					if (expectOperand) throw std::invalid_argument("Expected operand");
					expectOperand = false;
					while(!operatorStack.empty() && operatorStack.back().Type != OpenParen) {
						outputQueue.push_back(operatorStack.back());
						operatorStack.pop_back();
					}
					if(operatorStack.empty()) throw std::invalid_argument("Unbalanced Parens");
					operatorStack.pop_back(); // pop the open paren

					if(!operatorStack.empty() && operatorStack.back().Type == Function) {
						outputQueue.push_back(operatorStack.back());
						operatorStack.pop_back();
					}
					break;
				}
				case BinaryOperator: {
					if(expectOperand) throw std::invalid_argument("Expected operand");
					expectOperand = true;
					auto o1 = GetOperatorInfo(token);
					auto shouldPop = [&]() {
						if (operatorStack.empty()) return false;
						auto next = operatorStack.back();
						if (next.Type == OpenParen) return false;
						auto o2 = GetOperatorInfo(next);
						if (o1.Precedence != o2.Precedence) return o2.Precedence > o1.Precedence;
						return !o1.RightAssociative;
						};
					while (shouldPop()) {
						outputQueue.push_back(operatorStack.back());
						operatorStack.pop_back();
					}
					operatorStack.push_back(token);
					break;
				}
			}
		}
		if (expectOperand) throw std::invalid_argument("Expected operand");

		while (!operatorStack.empty()) {
			if (operatorStack.back().Type == ExpressionTokenType::OpenParen) throw std::invalid_argument("Unbalanced Parens");
			outputQueue.push_back(operatorStack.back());
			operatorStack.pop_back();
		}

		return outputQueue;
	}
}