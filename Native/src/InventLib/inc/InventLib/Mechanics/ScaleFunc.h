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
        constexpr T Specified(size_t currentLevel) {
            static std::vector<T> values = {Values...};
            return values[currentLevel];
        }

        template<typename T, T Value>
        constexpr T Constant(size_t) {
            return Value;
        }

        template<typename T, T Mult>
        constexpr T Linear(size_t currentLevel) {
            return static_cast<T>(currentLevel * Mult);
        }

        template<typename T, size_t Step, typename StepFn>
        auto MakePunctuated(StepFn stepFn) {
            return [stepFn](size_t currentLevel) { return stepFn((currentLevel / Step) + 1); };
        }

        template<typename T, size_t Base, typename StepFn>
        constexpr auto MakeLogPunctuated(StepFn stepFn) {
            return [stepFn](size_t currentLevel) {
                auto log = std::log(static_cast<double>(currentLevel)) / std::log(static_cast<double>(Base));
                return static_cast<T>(stepFn(static_cast<size_t>(log)));
            };
        };
    } // namespace AdvancementCosts
} // namespace Invent