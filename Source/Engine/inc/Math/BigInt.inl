#pragma once

#include "Constexpr/ConstexprStrUtils.h"

#include <cmath>

constexpr BigInt::BigInt(u64 coef, u32 exp, bool neg) {
    SetExponent(exp);
    SetNegative(neg);
    auto wideExp = static_cast<u64>(Exponent());
    while(coef > std::numeric_limits<u32>::max()) {
        coef /= 10;
        ++wideExp;
    }
    if(wideExp > std::numeric_limits<u32>::max() / 2) {
        m_Coef = std::numeric_limits<u32>::max();
        SetExponent(std::numeric_limits<u32>::max() / 2);
        return;
    }

    SetExponent(static_cast<u32>(wideExp));
    m_Coef = static_cast<u32>(coef);
    Normalize();
}

constexpr BigInt::BigInt(u32 val) : BigInt(static_cast<u64>(val)) {}
constexpr BigInt::BigInt(s32 val) : BigInt(static_cast<s64>(val)) {}

constexpr BigInt::BigInt(u64 val) : BigInt(val, 0, false) {}
constexpr BigInt::BigInt(s64 val) : BigInt(Mag(val), 0, val < 0) {}

constexpr BigInt BigInt::Pow10(u32 exponent) { return BigInt(1, exponent, false); }
constexpr BigInt BigInt::FromScientific(u64 coef, u32 exp) { return BigInt(coef, exp, false); }

constexpr bool BigInt::operator==(BigInt other) const {
    return m_Coef == other.m_Coef && m_ExpAndSign == other.m_ExpAndSign;
}

constexpr std::strong_ordering BigInt::operator<=>(const BigInt& other) const {
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

    auto lhs = static_cast<u64>(m_Coef);
    auto rhs = static_cast<u64>(other.m_Coef);
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

constexpr BigInt BigInt::operator-() const { 
    auto result = *this;
    if(result.m_Coef != 0) {
        result.SetNegative(!result.IsNegative());
    }
    return result;
}

constexpr BigInt& BigInt::operator+=(const BigInt& other) { 
    if(other.m_Coef == 0) return *this;
    if(m_Coef == 0) {
        *this = other;
        return *this;
    }

    auto Align = [](u64& loCoef, u64& loExp, u64& hiCoef, u64& hiExp) {
        constexpr auto limit = std::numeric_limits<u64>::max() / 2;

        while(loExp < hiExp && hiCoef <= limit / 10) {
            hiCoef *= 10;
            hiExp--;
        }
        while(loExp < hiExp && loCoef != 0) {
            loCoef /= 10;
            loExp++;
        }

        loExp = hiExp;
    };

    auto lhsCoef = static_cast<u64>(m_Coef);
    auto rhsCoef = static_cast<u64>(other.m_Coef);
    auto lhsExp = static_cast<u64>(Exponent());
    auto rhsExp = static_cast<u64>(other.Exponent());

    if(lhsExp < rhsExp) Align(lhsCoef, lhsExp, rhsCoef, rhsExp);
    else if(rhsExp < lhsExp) Align(rhsCoef, rhsExp, lhsCoef, lhsExp);

    auto exp = static_cast<u32>(lhsExp);
    if(IsNegative() == other.IsNegative()) {
        *this = BigInt(lhsCoef + rhsCoef, exp, IsNegative());
    } else if(lhsCoef >= rhsCoef) {
        *this = BigInt(lhsCoef - rhsCoef, exp, IsNegative());
    } else {
        *this = BigInt(rhsCoef - lhsCoef, exp, other.IsNegative());
    }

    return *this;
}

constexpr BigInt& BigInt::operator-=(const BigInt& other) {
    *this += -other;
    return *this;
}

constexpr BigInt& BigInt::operator*=(const BigInt& other) {
    if(m_Coef == 0 || other.m_Coef == 0) {
        *this = BigInt(0);
        return *this;
    }

    auto coef = static_cast<u64>(m_Coef) * static_cast<u64>(other.m_Coef);
    auto exp = static_cast<u64>(Exponent()) + static_cast<u64>(other.Exponent());
    auto neg = IsNegative() != other.IsNegative();

    if(exp > std::numeric_limits<u32>::max() / 2) {
        *this = neg ? MinValue : MaxValue;
    } else {
        *this = BigInt(coef, static_cast<u32>(exp), neg);
    }

    return *this;
}

constexpr BigInt& BigInt::operator/=(const BigInt& other) {
    if(other.m_Coef == 0) {
        throw std::domain_error("Divide by 0");
    }

    if(m_Coef == 0) return *this;
    auto neg = IsNegative() != other.IsNegative();
    auto expDiff = static_cast<s64>(Exponent()) - static_cast<s64>(other.Exponent());

    auto num = static_cast<u64>(m_Coef);
    while(expDiff > 0 && num <= std::numeric_limits<u64>::max() / 10) {
        num *= 10;
        expDiff--;
    }
    while(expDiff < 0) {
        num /= 10;
        expDiff++;
        if(num == 0) {
            *this = BigInt(0);
            return *this;
        }
    }

    auto q = num / other.m_Coef;
    *this = BigInt(q, static_cast<u32>(expDiff), neg);
    return *this;
}

constexpr BigInt& BigInt::Pow(u32 pow) {
    auto base = *this;
    auto result = BigInt(1);

    while(pow != 0) {
        if((pow & 1u) != 0) result *= base;

        pow >>= 1;
        if(pow != 0) base *= base;
    }

    *this = result;
    return *this;
}

template<std::floating_point TMul>
constexpr BigInt& BigInt::operator*=(TMul mul) {
    if(!(mul >= std::numeric_limits<TMul>::lowest() && mul <= std::numeric_limits<TMul>::max())) {
        throw std::domain_error("Invalid floating point value");
    }
    if(mul == 0.0) {
        *this = BigInt(0);
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

    auto coef = static_cast<f64>(m_Coef) * sig;
    auto exp = static_cast<s64>(Exponent()) + mulExp;
    auto neg = IsNegative() != negMul;

    while(exp > 0 && coef <= static_cast<f64>(std::numeric_limits<u32>::max()) / 10.0) {
        coef *= 10.0;
        --exp;
    }
    while(exp < 0 && coef > 0) {
        coef /= 10.0;
        ++exp;
    }

    *this = BigInt(static_cast<u64>(coef), static_cast<u32>(exp), neg);
    return *this;
}

static constexpr auto Suffixes = std::array{
    "K",  "M",  "B",  "T", "Qa", "Qi", "Sx", "Sp", "Oc", "No", "De",
    "Ud", "Dd", "Td", "Qad", "Qid", "Sxd", "Spd", "Ocd", "Nod", "Vn", 
    "Vu", "Vd", "Vt", "Vqa", "Vqi", "Vsx", "Vsp", "Voc", "Vnd", "Trd"
};
constexpr std::optional<std::string> BigInt::ToHumanReadable(size_t precision) const { 
    if(m_Coef == 0) return "0";
    auto mag = static_cast<u64>(Exponent()) + DigitCount() - 1;
    auto group = mag / 3;
    if(group > Suffixes.size()) return std::nullopt;

    auto digits = Constexpr::ToString(m_Coef);
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

constexpr std::string BigInt::ToScientific(size_t precision) const {
    if(m_Coef == 0) return "0e0";
    auto digits = Constexpr::ToString(m_Coef);
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

constexpr u64 BigInt::Mag(s64 val) {
    auto uVal = static_cast<u64>(val);
    return val < 0 ? u64{0} - uVal : uVal;
}

constexpr void BigInt::Normalize() {
    if(m_Coef == 0) {
        SetExponent(0);
        SetNegative(false);
        return;
    }

    auto coef = m_Coef;
    auto exp = static_cast<u64>(Exponent());
    while(coef % 10 == 0) {
        coef /= 10;
        ++exp;
    }
    if(exp > std::numeric_limits<u32>::max() / 2) {
        m_Coef = std::numeric_limits<u32>::max();
        SetExponent(std::numeric_limits<u32>::max() / 2);
        return;
    }

    m_Coef = coef;
    SetExponent(static_cast<u32>(exp));
}

constexpr u32 BigInt::DigitCount() const {
    u32 result = 0;
    auto running = m_Coef;
    while(running > 0) {
        running /= 10;
        result++;
    }
    return result;
}

inline constexpr BigInt BigInt::MaxValue =
    BigInt(std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max() / 2, false);
inline constexpr BigInt BigInt::MinValue =
    BigInt(std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max() / 2, true);
