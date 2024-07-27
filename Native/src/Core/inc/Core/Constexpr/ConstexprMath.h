#pragma once

#include <type_traits>

namespace Constexpr {
    template<typename T, typename PowType> constexpr T Pow(T val, PowType pow) {
        static_assert(!std::is_floating_point_v<PowType>, "This power only works with integers");
        if(pow == 0) return 1;

        T result = val;
        for(PowType i = 1; i < pow; i++) {
            result *= val;
        }
        return result;
    }

    constexpr auto FloatToPercent(auto value) {
        static_assert(std::is_floating_point_v<decltype(value)>, "FloatToPercent only supports floating point types");
        if(value >= 1.0) {
            // 1.1 -> 10%
            return (value - 1.0) * 100.0;
        } else {
            // 0.9 -> 10%
            return (1.0 - value) * 100.0;
        }
    }

    constexpr auto Abs(auto value) {
        if constexpr(std::is_signed_v<decltype(value)>) {
            return value < 0 ? -value : value;
        } else {
            return value;
        }
    }

}