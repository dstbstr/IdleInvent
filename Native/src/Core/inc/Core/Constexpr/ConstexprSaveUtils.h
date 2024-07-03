#pragma once

#include "Core/NumTypes.h"
#include "Core/Constexpr/ConstexprMath.h"

namespace Constexpr {
    static constexpr auto Base = 1.2;
    static constexpr auto InvBase = 1.0 / Base;

    constexpr u8 Compress(auto val) {
		if(val == 0) return 0;
		u8 power = 0;
        double running = static_cast<double>(val);
		while (running >= Base) {
            running *= InvBase;
            power++;
		}
        return power;
    }

    template<typename T>
    constexpr T Decompress(u8 val) { 
        if(val == 0) return (T)0;
        double result = 1;

        for (auto i = 0; i < val; i++) {
            result *= Base;
        }
        return static_cast<T>(result);
	}
}