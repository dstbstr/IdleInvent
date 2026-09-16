#pragma once

#include <Platform/NumTypes.h>

#include <array>
#include <compare>
#include <string>

template<size_t TBits, bool TSigned = false>
class FixedInt;
template<size_t TBits>
using UFixedInt = FixedInt<TBits, false>;
template<size_t TBits>
using SFixedInt = FixedInt<TBits, true>;
using u128 = UFixedInt<128>;
using s128 = SFixedInt<128>;

template<size_t TBits, bool TSigned>
class FixedInt {
public:
    constexpr FixedInt() = default;
    template<size_t TOtherBits>
        requires(TOtherBits <= TBits)
    constexpr FixedInt(const FixedInt<TOtherBits, TSigned>& other);
    constexpr FixedInt(u64 val);
    constexpr FixedInt(s64 val) requires(TSigned);
    constexpr FixedInt(s32 val) requires(TSigned) : FixedInt(static_cast<s64>(val)) {}

    static const FixedInt MinValue;
    static const FixedInt MaxValue;

    template<size_t TOtherBits>
    constexpr bool operator==(const FixedInt<TOtherBits, TSigned>& other) const;
    template<size_t TOtherBits>
    constexpr auto operator<=>(const FixedInt<TOtherBits, TSigned> & other) const;

    constexpr FixedInt operator-() const;
    constexpr FixedInt& operator+=(const FixedInt& other);
    constexpr FixedInt& operator-=(const FixedInt& other) { return *this += -other; }
    constexpr FixedInt& operator*=(const FixedInt& other);
    constexpr FixedInt& operator/=(const FixedInt& other);
    constexpr FixedInt& operator%=(const FixedInt& other);
    constexpr FixedInt& operator<<=(size_t shift);
    constexpr FixedInt& operator>>=(size_t shift);
    constexpr FixedInt operator~() const;
    constexpr FixedInt& operator&=(const FixedInt& other);
    constexpr FixedInt& operator|=(const FixedInt& other);
    constexpr FixedInt& operator^=(const FixedInt& other);

    friend constexpr FixedInt operator+(FixedInt lhs, const FixedInt& rhs) { return lhs += rhs; }
    friend constexpr FixedInt operator-(FixedInt lhs, const FixedInt& rhs) { return lhs -= rhs; }
    friend constexpr FixedInt operator*(FixedInt lhs, const FixedInt& rhs) { return lhs *= rhs; }
    friend constexpr FixedInt operator/(FixedInt lhs, const FixedInt& rhs) { return lhs /= rhs; }
    friend constexpr FixedInt operator%(FixedInt lhs, const FixedInt& rhs) { return lhs %= rhs; }
    friend constexpr FixedInt operator<<(FixedInt lhs, size_t shift) { return lhs <<= shift; }
    friend constexpr FixedInt operator>>(FixedInt lhs, size_t shift) { return lhs >>= shift; }
    friend constexpr FixedInt operator&(FixedInt lhs, const FixedInt& rhs) { return lhs &= rhs; }
    friend constexpr FixedInt operator|(FixedInt lhs, const FixedInt& rhs) { return lhs |= rhs; }
    friend constexpr FixedInt operator^(FixedInt lhs, const FixedInt& rhs) { return lhs ^= rhs; }

    constexpr std::string ToString() const;

private:
    template<size_t, bool>
    friend class FixedInt; // Support for FixedInt of different sizes to access each other's limbs

    static_assert(TBits > 1, "FixedInt must have at least 2 bit");
    static constexpr size_t LimbCount = TBits / 32 + (TBits % 32 != 0);
    static constexpr u32 SignMask = u32{1} << ((TBits - 1) % 32);

    constexpr bool IsNegative() const;
    constexpr u32 ExtendedLimb(size_t index) const;
    constexpr void ClearUnusedBits();
    constexpr std::pair<FixedInt, FixedInt> DivRem(const FixedInt& other) const;

    std::array<u32, LimbCount> m_Limbs{};
};

#include "FixedInt.inl"