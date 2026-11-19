#pragma once

#include "IduelTypes.h"
#include "Math/MathEval.h"
#include "Math/MathParser.h"

#include <string>

namespace Iduel {
    class Curve {
    public:
        Curve() = default;
        explicit Curve(const std::string& expr) {
			SetExpression(expr);
		}

		void SetExpression(const std::string& expr) {
			auto postfix = Math::ToPostfix(Math::Normalize(Math::Tokenize(expr)));
            m_Expression = expr;
			m_Postfix = std::move(postfix);
		}

        const std::string& GetExpression() const { return m_Expression; }
        f64 Evaluate(f64 x) const {
            return Math::Eval(m_Postfix, x);
        }

    private:
        std::string m_Expression{"1"};
        std::vector<Math::ExpressionToken> m_Postfix{{Math::ExpressionTokenType::Number, "1"}};
    };
}