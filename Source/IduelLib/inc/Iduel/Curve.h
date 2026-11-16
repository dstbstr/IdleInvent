#pragma once

#include "IduelTypes.h"

namespace Iduel {
    struct CurveTerm {
        f64 Coef{ 0.0 };
        bool ScaleX{ false };

        f64 Evaluate(f64 x) const {
            return Coef * (ScaleX ? x : 1.0);
        }
    };

    struct Curve {
        CurveTerm ConstTerm{ 0.0, false };
        CurveTerm LinearTerm{ 1.0, false };
        CurveTerm QuadTerm{ 1.0, false };
        CurveTerm ExpTerm{ 1.0, false };

        // A + B * (C ^ D)
        f64 Evaluate(f64 x) const {
            return ConstTerm.Evaluate(x) +
                LinearTerm.Evaluate(x) *
                std::pow(QuadTerm.Evaluate(x), ExpTerm.Evaluate(x));
        }
    };
}