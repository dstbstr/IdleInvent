#pragma once

#include <cmath>
#include <vector>

namespace Invent {
    namespace Scale {
        template<typename T, size_t Log>
        T Logarithmic(size_t currentLevel) {
            return static_cast<T>(std::pow(Log, currentLevel));
        }

        template<typename T, size_t Log, size_t FixedPoint>
        T FixedPointLog(size_t currentLevel) {
            return static_cast<T>(std::pow(Log / static_cast<double>(FixedPoint), currentLevel));
        }

        template<typename T, T... Values>
        T Specified(size_t currentLevel) {
            static std::vector<T> values = {Values...};
            return values[currentLevel];
        }

        template<typename T, T Value>
        T Constant(size_t) {
            return Value;
        }

        template<typename T, T Mult>
        T Linear(size_t currentLevel) {
            return static_cast<T>(currentLevel * Mult);
        }

        template<typename T, size_t Step, typename StepFn>
        auto MakePunctuated(StepFn stepFn) {
            return [stepFn](size_t currentLevel) { return stepFn((currentLevel / Step) + 1); };
        }
    } // namespace AdvancementCosts
} // namespace Invent