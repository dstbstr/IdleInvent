#pragma once

#include <Platform/NumTypes.h>
#include <functional>

struct Handle {
    u64 Value;
    constexpr void Reset() { Value = static_cast<u64>(-1); }
    constexpr Handle() : Value(static_cast<u64>(-1)) {}
    constexpr explicit Handle(u64 value) : Value(value) {}

    constexpr bool operator==(const Handle& other) const { return Value == other.Value; }
    constexpr bool operator!=(const Handle& other) const { return Value != other.Value; }

    constexpr operator bool() const { return Value != static_cast<u64>(-1); }
};
constexpr auto InvalidHandle = Handle(static_cast<u64>(-1));

namespace Handles {
    Handle Next();
}

namespace std {
    template<>
    struct hash<Handle> {
        size_t operator()(const Handle& handle) const noexcept {
            return std::hash<u64>{}(handle.Value);
        }
    };
}