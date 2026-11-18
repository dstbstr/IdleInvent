#pragma once

#include "Constexpr/ConstexprStrUtils.h"

#include <string_view>
#include <vector>

namespace Math {
	enum struct TokenType { Number, Identifier, Operator, OpenParen, CloseParen };
	struct Token {
		TokenType Type;
		std::string_view Text;

		constexpr bool operator==(const Token& other) const = default;
	};

	namespace TokenizerDetails {

		constexpr Token TokenizeNumber(std::string_view expr, size_t& pos) {
			auto start = pos;
			bool hasDigit = false;
			bool hasDot = false;

			while (pos < expr.size()) {
				auto next = expr[pos];
				if (Constexpr::IsDigit(next)) {
					hasDigit = true;
				}
				else if (next == '.') {
					if (hasDot) throw std::invalid_argument("Multiple decimal points");
					hasDot = true;
				}
				else {
					break;
				}
				pos++;
			}

			if (!hasDigit) throw std::invalid_argument("No digits in number");
			return { TokenType::Number, expr.substr(start, pos - start) };
		}

		constexpr Token TokenizeString(std::string_view expr, size_t& pos) {
			auto start = pos;
			while (pos < expr.size()) {
				auto next = expr[pos];
				if (Constexpr::IsAlnum(next) || next == '_') {
					pos++;
				}
				else {
					break;
				}
			}
			std::string_view text = expr.substr(start, pos - start);
			return { TokenType::Identifier, text };
		}

		constexpr Token TokenizeOperator(std::string_view expr, size_t& pos) {
			// TODO: Consider multi-character operators like "**"
			std::string_view text = expr.substr(pos++, 1);
			switch (text[0]) {
			case '(': return { TokenType::OpenParen, text };
			case ')': return { TokenType::CloseParen, text };
			case '+': case '-': case '*': case '/': case '^': return { TokenType::Operator, text };
			default: throw std::invalid_argument("Unknown operator: " + std::string(text));
			}
		}
	}

	constexpr std::vector<Token> Tokenize(std::string_view expr) {
		std::vector<Token> tokens;
		size_t pos = 0;

		while (pos < expr.size()) {
			auto c = expr[pos];
			if (Constexpr::IsSpace(c)) {
				pos++;
				continue;
			}

			if (Constexpr::IsDigit(c) || c == '.') {
				tokens.push_back(TokenizerDetails::TokenizeNumber(expr, pos));
			}
			else if (Constexpr::IsAlpha(c) || c == '_') {
				tokens.push_back(TokenizerDetails::TokenizeString(expr, pos));
			}
			else {
				tokens.push_back(TokenizerDetails::TokenizeOperator(expr, pos));
			}
		}

		return tokens;
	}
}