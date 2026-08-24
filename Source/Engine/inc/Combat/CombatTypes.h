#pragma once
#include <Platform/NumTypes.h>

#include <compare>
#include <limits>
#include <type_traits>
#include <utility>

namespace Combat {
	struct CombatantId {
        static constexpr u32 Invalid = std::numeric_limits<u32>::max();

        u32 Value {Invalid};
        [[nodiscard]] constexpr bool IsValid() const { return Value != Invalid; }

        constexpr explicit operator bool() const { return IsValid(); }
        constexpr auto operator<=>(const CombatantId&) const = default;
	};
}