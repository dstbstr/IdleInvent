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
}