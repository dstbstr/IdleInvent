#pragma once

#include <Platform/NumTypes.h>

#include <cmath>
#include <vector>

namespace Scale {
    // Exponential<u32, 2> == 2 ^ currentLevel
    // 2 4 8 16 32 ...
    template<typename T, size_t Base>
    T Exp(size_t currentLevel, T initialValue = T{1}) {
        if constexpr(requires(T value) { value.ScaleByPower(1.0, u32{}); }) {
            // if T has a ScaleByPower method, use it
            return initialValue.ScaleByPower(static_cast<f64>(Base), static_cast<u32>(currentLevel));
		}
		else {
			return static_cast<T>(initialValue * std::pow(Base, currentLevel));
		}
    }

    // FixedPointExp<f32, 12, 10> == 1.2 ^ currentLevel
    // 1 1.2 1.44 1.728 2.0736 ...
    template<typename T, size_t Base, size_t FixedPoint>
    T FixedPointExp(size_t currentLevel, T initialValue = T{1}) {
		static_assert(Base > 0.0, "Base must be greater than 0");
		static_assert(FixedPoint > 0, "FixedPoint must be greater than 0");
		double base = static_cast<double>(Base) / static_cast<double>(FixedPoint);

		if constexpr (requires(T value) { value.ScaleByPower(1.0, u32{}); }) {
			// if T has a ScaleByPower method, use it
			return initialValue.ScaleByPower(base, static_cast<u32>(currentLevel));
        } else {
            return static_cast<T>(initialValue * std::pow(base, currentLevel));
        }
    }

    // Specified<u32, 1, 3, 2, 5, 3> == [1,3,2,5,3]
    template<typename T, T... Values>
    constexpr T Specified(size_t currentLevel) {
        static std::vector<T> values = {Values...};
        return values[currentLevel];
    }

	//Constant<u32, 5> == 5 ...
    template<typename T, T Value>
    constexpr T Constant(size_t) {
        return Value;
    }

	// Linear<u32, 3> == 0, 3, 6, 9, 12 ...
    template<typename T, T Mult>
    constexpr T Linear(size_t currentLevel) {
        return static_cast<T>(currentLevel * Mult);
    }

	// MakePunctuated<u32, 3>(Scale::Linear<u32, 10>) ==
	// 10, 10, 10, 20, 20, 20, 30, 30, 30 ...
    template<typename T, size_t Step, typename StepFn>
    auto MakePunctuated(StepFn stepFn) {
        return [stepFn](size_t currentLevel) { return stepFn((currentLevel / Step) + 1); };
    }

	// MakeExponentialPunctuated<u32, 5, 10>(Scale::Linear<u32, 3>) ==
	// 3, 6, 9, 12, 15, 30, 60, 90, 120, 150, 300, 600 ...
    template<typename T, size_t TStepSize, size_t TStepGrowth, typename TStepFn>
    constexpr auto MakeExpPunctuated(TStepFn stepFn, T initialValue = T{1}) {
		static_assert(TStepSize > 0, "Step size must be greater than 0");
		static_assert(TStepGrowth > 0, "Step growth must be greater than 0");
        return [stepFn, initialValue](size_t currentLevel) {
            auto cycle = currentLevel / TStepSize;
            auto step = currentLevel % TStepSize + 1;
            auto value = initialValue * stepFn(step);
            return Exp<T, TStepGrowth>(cycle, value);
        };
    };
}
