#pragma once

#include "Utilities/Concepts.h"
#include <concepts>
#include <vector>

template<typename E>
concept CountEnum = std::is_enum_v<E> && requires(E e) {
    { E::COUNT } -> std::convertible_to<E>;
    { static_cast<size_t>(E::COUNT) } -> std::convertible_to<size_t>;
};

template<typename E>
concept UnsetEnum = std::is_enum_v<E> && requires(E e) {
    { E::Unset } -> std::convertible_to<E>;
};

template<typename E>
concept ToStringEnum = std::is_enum_v<E> && HasToString<E>;

template<typename E>
concept DescribeEnum = std::is_enum_v<E> && HasDescribe<E>;

namespace Enum {
    template<CountEnum E>
    constexpr E Begin() {
        if constexpr(UnsetEnum<E>) {
            return static_cast<E>(1);
        } else {
            return static_cast<E>(0);
        }
    }

    template<CountEnum E>
    constexpr E End() {
        return static_cast<E>(static_cast<size_t>(E::COUNT) - 1);
    }

    template<CountEnum E>
    constexpr E Increment(E e) {
        auto next = static_cast<size_t>(e) + 1;
        auto max = static_cast<size_t>(E::COUNT) - 1;
        return static_cast<E>(std::min(next, max));
    }

    template<CountEnum E>
    constexpr E Decrement(E e) {
        auto prev = static_cast<size_t>(e);
        if(prev > 0) {
            prev--;
        }

        auto min = 0ull;
        if constexpr(UnsetEnum<E>) {
            min = 1ull;
        }
        return static_cast<E>(std::max(prev, min));
    }

    template<CountEnum E>
    constexpr std::vector<E> GetAllValues() {
        std::vector<E> values;
        values.reserve(static_cast<size_t>(E::COUNT));
        for(auto i = Begin<E>(); i < End<E>(); i = Increment(i)) {
            values.push_back(i);
        }
        values.push_back(End<E>());
        return values;
    }
}