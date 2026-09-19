#pragma once
#include <Platform/NumTypes.h>
#include "Math/FixedInt.h"

#include <compare>
#include <concepts>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>

template<size_t TCoefBits = 32, size_t TExpBits = 31, bool TSigned = true>
class BigIntImpl;

using BigInt = BigIntImpl<>;

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
class BigIntImpl {
private:
    constexpr BigIntImpl(u128 coef, u32 exp, bool neg);

public:
    constexpr BigIntImpl();
    constexpr BigIntImpl(u32 val);
    constexpr BigIntImpl(s32 val);
    constexpr BigIntImpl(u64 val);
    constexpr BigIntImpl(s64 val);

    static constexpr BigIntImpl Pow10(u32 exponent);
    static constexpr BigIntImpl FromScientific(u64 coef, u32 exp);
    static f64 Ratio(const BigIntImpl& numerator, const BigIntImpl& denominator);

    constexpr bool operator==(BigIntImpl other) const;
    constexpr std::strong_ordering operator<=>(const BigIntImpl& other) const;

    constexpr BigIntImpl operator-() const;
    constexpr BigIntImpl& operator+=(const BigIntImpl& other);
    constexpr BigIntImpl& operator-=(const BigIntImpl& other);
    constexpr BigIntImpl& operator*=(const BigIntImpl& other);
    constexpr BigIntImpl& operator/=(const BigIntImpl& other);
    constexpr BigIntImpl& Pow(u32 pow);

    friend constexpr BigIntImpl operator+(BigIntImpl lhs, BigIntImpl rhs) { return lhs += rhs; }
    friend constexpr BigIntImpl operator-(BigIntImpl lhs, BigIntImpl rhs) { return lhs -= rhs; }
    friend constexpr BigIntImpl operator*(BigIntImpl lhs, BigIntImpl rhs) { return lhs *= rhs; }
    friend constexpr BigIntImpl operator/(BigIntImpl lhs, BigIntImpl rhs) { return lhs /= rhs; }

    template<std::floating_point TMul>
    constexpr BigIntImpl& operator*=(TMul mul);

    template<std::floating_point TMul>
    friend constexpr BigIntImpl operator*(BigIntImpl lhs, TMul rhs) {
        return lhs *= rhs;
    }
    template<std::floating_point TMul>
    friend constexpr BigIntImpl operator*(TMul lhs, BigIntImpl rhs) {
        return rhs *= lhs;
    }

    constexpr std::optional<std::string> ToHumanReadable(size_t precision = 2, size_t scale = 0) const;
    constexpr std::string ToScientific(size_t precision = 2, size_t scale = 0) const;
    static constexpr std::string ToTimeString(const BigIntImpl& milliseconds);

    static const BigIntImpl MaxValue;
    static const BigIntImpl MinValue;

private:
    static_assert(TCoefBits > 0 && TCoefBits <= 64); // TODO: consider relaxing this
    static_assert(TExpBits > 0 && TExpBits <= 32);
    using Storage = UFixedInt<TCoefBits + TExpBits + TSigned>;
    using Coefficient = UFixedInt<TCoefBits>;

    Storage m_Storage{};

    static constexpr Storage ExponentMask = (Storage{1} << TExpBits) - Storage{1};
    static constexpr Storage CoefMask = (Storage{1} << TCoefBits) - Storage{1};
    static constexpr Storage SignMask = [] {
        if constexpr(TSigned) {
            return Storage{1} << (TCoefBits + TExpBits);
        } else {
            return Storage{};
        }
    }();

    constexpr u32 Exponent() const {
        auto bits = (m_Storage >> TCoefBits) & ExponentMask;
        return static_cast<u32>(bits.ToU64());
    }
    constexpr void SetExponent(u32 exp) {
        if(static_cast<u64>(exp) > ExponentMask.ToU64()) throw "Bad input";

        auto fieldMask = ExponentMask << TCoefBits;
        m_Storage = (m_Storage & ~fieldMask) | (Storage{exp} << TCoefBits);
    }

    constexpr u64 Coef() const { return (m_Storage & CoefMask).ToU64(); }
    constexpr void SetCoef(u64 coef) {
        if(coef > CoefMask.ToU64()) throw "Bad input";
        m_Storage = (m_Storage & ~CoefMask) | Storage{coef};
    }

    constexpr bool IsNegative() const { return (m_Storage & SignMask) != Storage{}; }
    constexpr void SetNegative(bool negative) {
        if constexpr(TSigned) {
            m_Storage = (m_Storage & ~SignMask) | (negative ? SignMask : Storage{});
        } else if(negative) {
            throw std::domain_error("Cannot set negative on unsigned BigInt");
        }
    }
    static constexpr u64 Mag(s64 val);
    constexpr void Normalize();
    constexpr u32 DigitCount() const;

    constexpr static void Align(u128& loCoef, u64& loExp, u128& hiCoef, u64& hiExp);
};

#include "BigInt.inl"