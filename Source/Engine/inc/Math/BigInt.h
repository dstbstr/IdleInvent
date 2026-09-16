#pragma once
#include <Platform/NumTypes.h>

#include <limits>
#include <compare>
#include <concepts>
#include <optional>
#include <string>

class BigInt {
private:
    constexpr BigInt(u64 coef, u32 exp, bool neg);

public:
    constexpr BigInt(u32 val);
    constexpr BigInt(s32 val);
    constexpr BigInt(u64 val);
    constexpr BigInt(s64 val);

    static constexpr BigInt Pow10(u32 exponent);
    static constexpr BigInt FromScientific(u64 coef, u32 exp);

    constexpr bool operator==(BigInt other) const;
    constexpr std::strong_ordering operator<=>(const BigInt& other) const;

    constexpr BigInt operator-() const;
    constexpr BigInt& operator+=(const BigInt& other);
    constexpr BigInt& operator-=(const BigInt& other);
    constexpr BigInt& operator*=(const BigInt& other);
    constexpr BigInt& operator/=(const BigInt& other);
    constexpr BigInt& Pow(u32 pow);

    friend constexpr BigInt operator+(BigInt lhs, BigInt rhs) { return lhs += rhs; }
    friend constexpr BigInt operator-(BigInt lhs, BigInt rhs) { return lhs += -rhs; }
    friend constexpr BigInt operator*(BigInt lhs, BigInt rhs) { return lhs *= rhs; }
    friend constexpr BigInt operator/(BigInt lhs, BigInt rhs) { return lhs /= rhs; }

    template<std::floating_point TMul>
    constexpr BigInt& operator*=(TMul mul);

    template<std::floating_point TMul>
    friend constexpr BigInt operator*(BigInt lhs, TMul rhs) {return lhs *= rhs;}
    template<std::floating_point TMul>
    friend constexpr BigInt operator*(TMul lhs, BigInt rhs) { return rhs *= lhs; }

    constexpr std::optional<std::string> ToHumanReadable(size_t precision = 2) const;
    constexpr std::string ToScientific(size_t precision = 2) const;

    static const BigInt MaxValue;
    static const BigInt MinValue;

private:
    u32 m_Coef{};
    u32 m_ExpAndSign{};

    static constexpr u32 SignMask = u32{1} << 31;
    static constexpr u32 ExponentMask = SignMask - 1;

    constexpr u32 Exponent() const { return m_ExpAndSign & ExponentMask; }
    constexpr void SetExponent(u32 exp) {
        if(exp > ExponentMask) throw "Bad input";
        m_ExpAndSign = (m_ExpAndSign & SignMask) | exp;
    }
    constexpr bool IsNegative() const { return (m_ExpAndSign & SignMask) != 0; }
    constexpr void SetNegative(bool negative) { 
        m_ExpAndSign = (m_ExpAndSign & ExponentMask) | (negative ? SignMask : 0u);
    }
    static constexpr u64 Mag(s64 val);
    constexpr void Normalize();
    constexpr u32 DigitCount() const;
};

#include "BigInt.inl"