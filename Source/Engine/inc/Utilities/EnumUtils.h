#pragma once

#include <concepts>
#include <vector>

template<typename E>
concept CountEnum = std::is_enum_v<E> && requires(E e) {
    { E::COUNT } -> std::convertible_to<E>;
    { static_cast<size_t>(E::COUNT) } -> std::convertible_to<size_t>;
};

template<typename E>
concept ToStringEnum = std::is_enum_v<E> && requires(E e) {
    { ToString(e) } -> std::convertible_to<std::string>;
};

template<typename E>
concept DescribeEnum = std::is_enum_v<E> && requires(E e) {
    { Describe(e) } -> std::convertible_to<std::string>;
};

template<typename E>
concept UnsetEnum = std::is_enum_v<E> && requires(E e) {
    { E::Unset } -> std::convertible_to<E>;
};

namespace Enum {
    template<CountEnum E>
    std::vector<E> GetAllValues() {
        constexpr size_t count = static_cast<size_t>(E::COUNT);
        std::vector<E> values;
        values.reserve(count);
        size_t i = 0;
        if constexpr(UnsetEnum<E>) {
            i++; // skip Unset
        }
        for(; i < count; ++i) {
            values.push_back(static_cast<E>(i));
        }
        return values;
    }

    template<CountEnum E>
    E Increment(E e) {
        auto next = static_cast<size_t>(e) + 1;
        auto max = static_cast<size_t>(E::COUNT) - 1;
        return static_cast<E>(std::min(next, max));
    }

    template<CountEnum E>
    E Decrement(E e) {
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
}