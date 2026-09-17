#pragma once

#include "Constexpr/ConstexprStrUtils.h"

#include <cmath>

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr BigIntImpl<TCoefBits, TExpBits, TSigned>::BigIntImpl(u128 coef, u32 exp, bool neg) {
    SetNegative(neg);
    auto wideExp = static_cast<u64>(exp);
    while(coef.BitWidth() > TCoefBits) {
        coef /= 10;
        ++wideExp;
    }
    if(wideExp > ExponentMask.ToU64()) {
        SetCoef(CoefMask.ToU64());
        SetExponent(static_cast<u32>(ExponentMask.ToU64()));
        return;
    }

    SetExponent(static_cast<u32>(wideExp));
    SetCoef(coef.ToU64());
    Normalize();
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr BigIntImpl<TCoefBits, TExpBits, TSigned>::BigIntImpl(u32 val) : BigIntImpl(static_cast<u64>(val)) {}
template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr BigIntImpl<TCoefBits, TExpBits, TSigned>::BigIntImpl(s32 val) : BigIntImpl(static_cast<s64>(val)) {}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr BigIntImpl<TCoefBits, TExpBits, TSigned>::BigIntImpl(u64 val) : BigIntImpl(val, 0, false) {}
template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr BigIntImpl<TCoefBits, TExpBits, TSigned>::BigIntImpl(s64 val) : BigIntImpl(Mag(val), 0, val < 0) {}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::Pow10(u32 exponent) -> BigIntImpl {
    return BigIntImpl(1, exponent, false);
}
template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::FromScientific(u64 coef, u32 exp) -> BigIntImpl {
    return BigIntImpl(coef, exp, false);
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr bool BigIntImpl<TCoefBits, TExpBits, TSigned>::operator==(BigIntImpl other) const {
    return m_Storage == other.m_Storage;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr std::strong_ordering BigIntImpl<TCoefBits, TExpBits, TSigned>::operator<=>(const BigIntImpl& other) const {
    if(IsNegative() != other.IsNegative()) {
        return IsNegative() ? std::strong_ordering::less : std::strong_ordering::greater;
    }
    if(*this == other) return std::strong_ordering::equal;
    auto lhsDigits = DigitCount();
    auto rhsDigits = other.DigitCount();
    auto lhsMag = static_cast<u64>(Exponent()) + lhsDigits;
    auto rhsMag = static_cast<u64>(other.Exponent()) + rhsDigits;
    if(lhsMag != rhsMag) {
        return IsNegative() ? rhsMag <=> lhsMag : lhsMag <=> rhsMag;
    }

    auto lhs = static_cast<u64>(Coef());
    auto rhs = static_cast<u64>(other.Coef());
    while(lhsDigits < rhsDigits) {
        lhs *= 10;
        ++lhsDigits;
    }
    while(rhsDigits < lhsDigits) {
        rhs *= 10;
        ++rhsDigits;
    }

    return IsNegative() ? rhs <=> lhs : lhs <=> rhs;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator-() const -> BigIntImpl { 
    auto result = *this;
    if(result.Coef() != 0) {
        result.SetNegative(!result.IsNegative());
    }
    return result;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator+=(const BigIntImpl& other) -> BigIntImpl& { 
    if(other.Coef() == 0) return *this;
    if(Coef() == 0) {
        *this = other;
        return *this;
    }

    auto Align = [](u128& loCoef, u64& loExp, u128& hiCoef, u64& hiExp) {
        constexpr auto limit = u128::MaxValue >> 1;

        while(loExp < hiExp && hiCoef <= limit / 10) {
            hiCoef *= 10;
            hiExp--;
        }
        while(loExp < hiExp && loCoef != u128{}) {
            loCoef /= 10;
            loExp++;
        }

        loExp = hiExp;
    };

    auto lhsCoef = u128(Coef());
    auto rhsCoef = u128(other.Coef());
    auto lhsExp = static_cast<u64>(Exponent());
    auto rhsExp = static_cast<u64>(other.Exponent());

    if(lhsExp < rhsExp) Align(lhsCoef, lhsExp, rhsCoef, rhsExp);
    else if(rhsExp < lhsExp) Align(rhsCoef, rhsExp, lhsCoef, lhsExp);

    auto exp = static_cast<u32>(lhsExp);
    if(IsNegative() == other.IsNegative()) {
        *this = BigIntImpl(lhsCoef + rhsCoef, exp, IsNegative());
    } else if(lhsCoef >= rhsCoef) {
        *this = BigIntImpl(lhsCoef - rhsCoef, exp, IsNegative());
    } else {
        *this = BigIntImpl(rhsCoef - lhsCoef, exp, other.IsNegative());
    }

    return *this;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator-=(const BigIntImpl & other) ->BigIntImpl& {
    *this += -other;
    return *this;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator*=(const BigIntImpl& other) -> BigIntImpl& {
    if(Coef() == 0 || other.Coef() == 0) {
        *this = BigIntImpl(0);
        return *this;
    }

    auto coef = u128(Coef()) * u128(other.Coef()); 
    auto exp = static_cast<u64>(Exponent()) + static_cast<u64>(other.Exponent());
    auto neg = IsNegative() != other.IsNegative();

    if(exp > ExponentMask.ToU64()) {
        *this = neg ? MinValue : MaxValue;
    } else {
        *this = BigIntImpl(coef, static_cast<u32>(exp), neg);
    }

    return *this;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator/=(const BigIntImpl& other) -> BigIntImpl& {
    if(other.Coef() == 0) {
        throw std::domain_error("Divide by 0");
    }

    if(Coef() == 0) return *this;
    auto neg = IsNegative() != other.IsNegative();
    auto expDiff = static_cast<s64>(Exponent()) - static_cast<s64>(other.Exponent());

    auto num = static_cast<u64>(Coef());
    while(expDiff > 0 && num <= std::numeric_limits<u64>::max() / 10) {
        num *= 10;
        expDiff--;
    }
    while(expDiff < 0) {
        num /= 10;
        expDiff++;
        if(num == 0) {
            *this = BigIntImpl(0);
            return *this;
        }
    }

    auto q = num / other.Coef();
    *this = BigIntImpl(q, static_cast<u32>(expDiff), neg);
    return *this;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::Pow(u32 pow) -> BigIntImpl& {
    auto base = *this;
    auto result = BigIntImpl(1);

    while(pow != 0) {
        if((pow & 1u) != 0) result *= base;

        pow >>= 1;
        if(pow != 0) base *= base;
    }

    *this = result;
    return *this;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
template<std::floating_point TMul>
constexpr auto BigIntImpl<TCoefBits, TExpBits, TSigned>::operator*=(TMul mul) -> BigIntImpl& {
    if(!(mul >= std::numeric_limits<TMul>::lowest() && mul <= std::numeric_limits<TMul>::max())) {
        throw std::domain_error("Invalid floating point value");
    }
    if(mul == 0.0) {
        *this = BigIntImpl(0);
        return *this;
    }

    auto negMul = mul < 0;
    auto sig = static_cast<f64>(mul);
    if(negMul) sig = -sig;

    s64 mulExp = 0;

    while(sig >= 10.0) {
        sig /= 10.0;
        ++mulExp;
    }
    while(sig < 1.0) {
        sig *= 10.0;
        --mulExp;
    }

    auto coef = static_cast<f64>(Coef()) * sig;
    auto exp = static_cast<s64>(Exponent()) + mulExp;
    auto neg = IsNegative() != negMul;

    while(exp > 0 && coef <= static_cast<f64>(CoefMask.ToU64()) / 10.0) {
        coef *= 10.0;
        --exp;
    }
    while(exp < 0 && coef > 0) {
        coef /= 10.0;
        ++exp;
    }

    *this = BigIntImpl(static_cast<u64>(coef), static_cast<u32>(exp), neg);
    return *this;
}

static constexpr auto Suffixes = std::array{
    "K",  "M",  "B",  "T", "Qa", "Qi", "Sx", "Sp", "Oc", "No", "De",
    "Ud", "Dd", "Td", "Qad", "Qid", "Sxd", "Spd", "Ocd", "Nod", "Vn", 
    "Vu", "Vd", "Vt", "Vqa", "Vqi", "Vsx", "Vsp", "Voc", "Vnd", "Trd"
};
template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr std::optional<std::string> BigIntImpl<TCoefBits, TExpBits, TSigned>::ToHumanReadable(size_t precision) const { 
    if(Coef() == 0) return "0";
    auto mag = static_cast<u64>(Exponent()) + DigitCount() - 1;
    auto group = mag / 3;
    if(group > Suffixes.size()) return std::nullopt;

    auto digits = Constexpr::ToString(Coef());
    auto wholeDigits = static_cast<size_t>(mag % 3 + 1);
    std::string result;
    if(IsNegative()) result += '-';

    for(size_t i = 0; i < wholeDigits; i++) {
        result += i < digits.size() ? digits[i] : '0';
    }

    auto fracDigits = digits.size() - wholeDigits;
    if(precision > 0) {
        result += '.';
        for(size_t i = 0; i < precision; i++) {
            auto digit = i + wholeDigits;
            result.push_back(digit < digits.size() ? digits[digit] : '0');
        }
    }

    if(group > 0) {
        result += Suffixes[static_cast<size_t>(group - 1)];
    }

    return result;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr std::string BigIntImpl<TCoefBits, TExpBits, TSigned>::ToScientific(size_t precision) const {
    if(Coef() == 0) return "0e0";
    auto digits = Constexpr::ToString(Coef());
    std::string result;
    result.reserve(precision + 5);
    result += digits.substr(0, precision + 1);
    while(result.size() < precision + 1) {
        result.push_back('0');
    }

    result.insert(result.begin() + 1, '.');
    result.push_back('e');

    auto exponent = static_cast<u64>(Exponent()) + digits.size() - 1;
    result += Constexpr::ToString(exponent);
    if(IsNegative()) result.insert(result.begin(), '-');
    return result;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr u64 BigIntImpl<TCoefBits, TExpBits, TSigned>::Mag(s64 val) {
    auto uVal = static_cast<u64>(val);
    return val < 0 ? u64{0} - uVal : uVal;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr void BigIntImpl<TCoefBits, TExpBits, TSigned>::Normalize() {
    if(Coef() == 0) {
        SetExponent(0);
        SetNegative(false);
        return;
    }

    auto coef = Coef();
    auto exp = static_cast<u64>(Exponent());
    while(coef % 10 == 0) {
        coef /= 10;
        ++exp;
    }
    if(exp > ExponentMask.ToU64()) {
        SetCoef(CoefMask.ToU64());
        SetExponent(static_cast<u32>(ExponentMask.ToU64()));
        return;
    }

    SetCoef(coef);
    SetExponent(static_cast<u32>(exp));
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
constexpr u32 BigIntImpl<TCoefBits, TExpBits, TSigned>::DigitCount() const {
    u32 result = 0;
    auto running = Coef();
    while(running > 0) {
        running /= 10;
        result++;
    }
    return result;
}

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
inline constexpr BigIntImpl<TCoefBits, TExpBits, TSigned> BigIntImpl<TCoefBits, TExpBits, TSigned>::MaxValue =
    BigIntImpl(CoefMask.ToU64(), static_cast<u32>(ExponentMask.ToU64()), false);

template<size_t TCoefBits, size_t TExpBits, bool TSigned>
inline constexpr BigIntImpl<TCoefBits, TExpBits, TSigned> BigIntImpl<TCoefBits, TExpBits, TSigned>::MinValue = [] {
    if constexpr(TSigned) {
        return BigIntImpl(CoefMask.ToU64(), static_cast<u32>(ExponentMask.ToU64()), true);
    } else {
        return BigIntImpl{0};
    }
}();