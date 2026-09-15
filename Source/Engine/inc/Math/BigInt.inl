#pragma once

#include <cmath>

constexpr BigInt::BigInt(u64 coef, u32 exp, bool neg) : m_Exp(exp), m_Neg(neg) {
    auto wideExp = static_cast<u64>(m_Exp);
    while(coef > std::numeric_limits<u32>::max()) {
        coef /= 10;
        ++wideExp;
    }
    if(wideExp > std::numeric_limits<u32>::max()) {
        m_Coef = std::numeric_limits<u32>::max();
        m_Exp = std::numeric_limits<u32>::max();
        return;
    }

    m_Exp = static_cast<u32>(wideExp);
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
    return m_Neg == other.m_Neg && m_Coef == other.m_Coef && m_Exp == other.m_Exp;
}

constexpr std::strong_ordering BigInt::operator<=>(const BigInt& other) const {
    if(m_Neg != other.m_Neg) {
        return m_Neg ? std::strong_ordering::less : std::strong_ordering::greater;
    }
    if(*this == other) return std::strong_ordering::equal;
    auto lhsDigits = DigitCount();
    auto rhsDigits = other.DigitCount();
    auto lhsMag = static_cast<u64>(m_Exp) + lhsDigits;
    auto rhsMag = static_cast<u64>(other.m_Exp) + rhsDigits;
    if(lhsMag != rhsMag) {
        return m_Neg ? rhsMag <=> lhsMag : lhsMag <=> rhsMag;
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

    return m_Neg ? rhs <=> lhs : lhs <=> rhs;
}

constexpr BigInt BigInt::operator-() const { 
    auto result = *this;
    if(result.m_Coef != 0) {
        result.m_Neg = !result.m_Neg;
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
        auto limit = std::numeric_limits<u64>::max() / 2;

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
    auto lhsExp = static_cast<u64>(m_Exp);
    auto rhsExp = static_cast<u64>(other.m_Exp);

    if(lhsExp < rhsExp) Align(lhsCoef, lhsExp, rhsCoef, rhsExp);
    else if(rhsExp < lhsExp) Align(rhsCoef, rhsExp, lhsCoef, lhsExp);

    auto exp = static_cast<u32>(lhsExp);
    if(m_Neg == other.m_Neg) {
        *this = BigInt(lhsCoef + rhsCoef, exp, m_Neg);
    } else if(lhsCoef >= rhsCoef) {
        *this = BigInt(lhsCoef - rhsCoef, exp, m_Neg);
    } else {
        *this = BigInt(rhsCoef - lhsCoef, exp, other.m_Neg);
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
    auto exp = static_cast<u64>(m_Exp) + static_cast<u64>(other.m_Exp);
    auto neg = m_Neg != other.m_Neg;

    if(exp > std::numeric_limits<u32>::max()) {
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
    auto neg = m_Neg != other.m_Neg;
    auto expDiff = static_cast<s64>(m_Exp) - static_cast<s64>(other.m_Exp);

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
    auto exp = static_cast<s64>(m_Exp) + mulExp;
    auto neg = m_Neg != negMul;

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


constexpr u64 BigInt::Mag(s64 val) {
    auto uVal = static_cast<u64>(val);
    return val < 0 ? u64{0} - uVal : uVal;
}

constexpr void BigInt::Normalize() {
    if(m_Coef == 0) {
        m_Exp = 0;
        m_Neg = false;
        return;
    }

    auto coef = m_Coef;
    auto exp = static_cast<u64>(m_Exp);
    while(coef % 10 == 0) {
        coef /= 10;
        ++exp;
    }
    if(exp > std::numeric_limits<u32>::max()) {
        m_Coef = std::numeric_limits<u32>::max();
        m_Exp = std::numeric_limits<u32>::max();
        return;
    }

    m_Coef = coef;
    m_Exp = static_cast<u32>(exp);
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
    BigInt(std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max(), false);
inline constexpr BigInt BigInt::MinValue =
    BigInt(std::numeric_limits<u32>::max(), std::numeric_limits<u32>::max(), true);
