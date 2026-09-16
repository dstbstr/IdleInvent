#include <algorithm>
#include <bit>
#include <limits>
#include <stdexcept>


template<size_t TBits, bool TSigned>
template<size_t TOtherBits>
    requires(TOtherBits <= TBits)
constexpr FixedInt<TBits, TSigned>::FixedInt(const FixedInt<TOtherBits, TSigned>& other) {
    auto negative = other.IsNegative();
    static constexpr auto ones = std::numeric_limits<u32>::max();
    m_Limbs.fill(negative ? ones : 0u);

    for(size_t i = 0; i < other.m_Limbs.size(); ++i) {
        m_Limbs[i] = other.m_Limbs[i];
    }

    if(negative) {
        if constexpr(TOtherBits % 32 != 0) {
            m_Limbs[other.m_Limbs.size() - 1] |= (ones << (TOtherBits % 32));
        }
    }

    ClearUnusedBits();
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>::FixedInt(u64 val) {
    if(static_cast<size_t>(std::bit_width(val)) + TSigned > TBits) {
        throw std::overflow_error("Value too large for FixedInt");
    }

    m_Limbs[0] = static_cast<u32>(val);
    if constexpr(LimbCount > 1) {
        m_Limbs[1] = static_cast<u32>(val >> 32);
    }
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>::FixedInt(s64 val) requires (TSigned) {
    if constexpr(TBits < 64) {
        auto limit = s64{1} << (TBits - 1);
        if(val < -limit || val >= limit) {
            throw std::overflow_error("Not enough bits to store value");
        }
    }

    m_Limbs.fill(val < 0 ? std::numeric_limits<u32>::max() : 0u);

    auto bits = static_cast<u64>(val);
    m_Limbs[0] = static_cast<u32>(bits);
    if constexpr(LimbCount > 1) {
        m_Limbs[1] = static_cast<u32>(bits >> 32);
    }

    ClearUnusedBits();
}

template<size_t TBits, bool TSigned>
template<size_t TOtherBits>
constexpr bool FixedInt<TBits, TSigned>::operator==(const FixedInt<TOtherBits, TSigned>& other) const {
    if(IsNegative() != other.IsNegative()) return false;

    auto count = std::max(m_Limbs.size(), other.m_Limbs.size());

    for(size_t i = 0; i < count; i++) {
        auto lhs = ExtendedLimb(i);
        auto rhs = other.ExtendedLimb(i);
        if(lhs != rhs) return false;
    }

    return true;
}

template<size_t TBits, bool TSigned>
template<size_t TOtherBits>
constexpr auto FixedInt<TBits, TSigned>::operator<=>(const FixedInt<TOtherBits, TSigned>& other) const {
    if(IsNegative() != other.IsNegative()) {
        return IsNegative() ? std::strong_ordering::less : std::strong_ordering::greater;
    }

    auto count = std::max(m_Limbs.size(), other.m_Limbs.size());
    for(size_t i = count; i > 0;) {
        i--;
        auto lhs = ExtendedLimb(i);
        auto rhs = other.ExtendedLimb(i);
        if(auto order = lhs <=> rhs; order != std::strong_ordering::equal) {
            return order;
        }
    }

    return std::strong_ordering::equal;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned> FixedInt<TBits, TSigned>::operator-() const {
    auto result = *this;
    u64 carry = 1;

    for(auto& limb : result.m_Limbs) {
        auto inverted = static_cast<u32>(~limb);
        auto sum = static_cast<u64>(inverted) + carry;
        limb = static_cast<u32>(sum);
        carry = sum >> 32;
    }

    result.ClearUnusedBits();
    return result;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator+=(const FixedInt& other) {
    u64 carry = 0;

    for(size_t i = 0; i < LimbCount; i++) {
        auto sum = static_cast<u64>(m_Limbs[i]) + static_cast<u64>(other.m_Limbs[i]) + carry;
        m_Limbs[i] = static_cast<u32>(sum);
        carry = sum >> 32;
    }

    ClearUnusedBits();
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator*=(const FixedInt& other) {
    FixedInt result{};

    for(size_t i = 0; i < LimbCount; i++) {
        u64 carry = 0;

        for(size_t j = 0; j < LimbCount - i; j++) {
            auto index = i + j;
            auto product = static_cast<u64>(m_Limbs[i]) * other.m_Limbs[j];
            auto sum = product + result.m_Limbs[index] + carry;

            result.m_Limbs[index] = static_cast<u32>(sum);
            carry = sum >> 32;
        }
    }

    result.ClearUnusedBits();
    *this = result;
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator/=(const FixedInt& other) {
    *this = DivRem(other).first;
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator%=(const FixedInt& other) {
    *this = DivRem(other).second;
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator<<=(size_t shift) {
    FixedInt result{};
    if(shift < TBits) {
        auto words = shift / 32;
        auto bits = shift % 32;

        for(size_t i = words; i < LimbCount; i++) {
            auto source = i - words;
            result.m_Limbs[i] = m_Limbs[source] << bits;
            if(bits != 0 && source > 0) {
                result.m_Limbs[i] |= m_Limbs[source - 1] >> (32 - bits);
            }
        }
    }

    result.ClearUnusedBits();
    *this = result;
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator>>=(size_t shift) {
    FixedInt result{};
    if(shift >= TBits) {
        result.m_Limbs.fill(IsNegative() ? std::numeric_limits<u32>::max() : 0u);
    } else {
        auto words = shift / 32;
        auto bits = shift % 32;

        for(size_t i = 0; i < LimbCount; i++) {
            auto source = i + words;
            result.m_Limbs[i] = ExtendedLimb(source) >> bits;
            if(bits != 0 ) {
                result.m_Limbs[i] |= ExtendedLimb(source + 1) << (32 - bits);
            }
        }
    }

    result.ClearUnusedBits();
    *this = result;
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned> FixedInt<TBits, TSigned>::operator~() const {
    auto result = *this;
    for(auto& limb : result.m_Limbs) {
        limb = static_cast<u32>(~limb);
    }
    result.ClearUnusedBits();
    return result;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator&=(const FixedInt<TBits, TSigned>& other) {
    for(size_t i =0; i < LimbCount; i++) {
        m_Limbs[i] &= other.m_Limbs[i];
    }
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator|=(const FixedInt<TBits, TSigned>& other) {
    for(size_t i = 0; i < LimbCount; i++) {
        m_Limbs[i] |= other.m_Limbs[i];
    }
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr FixedInt<TBits, TSigned>& FixedInt<TBits, TSigned>::operator^=(const FixedInt<TBits, TSigned>& other) {
    for(size_t i = 0; i < LimbCount; i++) {
        m_Limbs[i] ^= other.m_Limbs[i];
    }
    return *this;
}

template<size_t TBits, bool TSigned>
constexpr std::pair<FixedInt<TBits, TSigned>, FixedInt<TBits, TSigned>> FixedInt<TBits, TSigned>::DivRem(const FixedInt& other) const {
    if(other == FixedInt{}) throw std::domain_error("Division by 0");

    auto negative = IsNegative() != other.IsNegative();

    UFixedInt<TBits> numer{};
    UFixedInt<TBits> denom{};
    numer.m_Limbs = m_Limbs;
    denom.m_Limbs = other.m_Limbs;

    if(IsNegative()) numer = -numer;
    if(other.IsNegative()) denom = -denom;

    UFixedInt<TBits> q{};
    UFixedInt<TBits> r{};

    for(size_t bit = TBits; bit > 0;) {
        --bit;
        r += r;
        auto incoming = (numer.m_Limbs[bit / 32] >> (bit % 32)) & 1u;
        r.m_Limbs[0] |= incoming;

        if(r >= denom) {
            r -= denom;
            q.m_Limbs[bit / 32] |= (1u << (bit % 32));
        }
    }

    if(negative) q = -q;
    if(IsNegative()) r = -r;
    FixedInt quotient{};
    FixedInt remainder{};
    quotient.m_Limbs = q.m_Limbs;
    remainder.m_Limbs = r.m_Limbs;
    return {quotient, remainder};
}

template<size_t TBits, bool TSigned>
constexpr bool FixedInt<TBits, TSigned>::IsNegative() const {
    if constexpr(TSigned) {
        return (m_Limbs.back() & SignMask) != 0;
    } else {
        return false;
    }
}

template<size_t TBits, bool TSigned>
constexpr u32 FixedInt<TBits, TSigned>::ExtendedLimb(size_t index) const {
    auto negative = IsNegative();
    static constexpr auto ones = std::numeric_limits<u32>::max();

    if(index >= LimbCount) return negative ? ones : 0u;

    auto limb = m_Limbs[index];
    if constexpr(TBits % 32 != 0) {
        if(negative && index == LimbCount - 1) {
            limb |= (ones << (TBits % 32));
        }
    }
    return limb;
}

template<size_t TBits, bool TSigned>
constexpr std::string FixedInt<TBits, TSigned>::ToString() const {
    auto neg = IsNegative();
    UFixedInt<TBits> mag;
    mag.m_Limbs = m_Limbs;
    if(neg) mag = -mag;

    std::string result;
    result.reserve(TBits / 3 + 2);

    do {
        u64 remainder = 0;
        for(size_t i = LimbCount; i > 0;) {
            --i;
            auto current = (remainder << 32) | mag.m_Limbs[i];
            mag.m_Limbs[i] = static_cast<u32>(current / 10);
            remainder = current % 10;
        }

        result.push_back(static_cast<char>('0' + remainder));
    } while(mag != UFixedInt<TBits>{});

    if(neg) result.push_back('-');
    std::reverse(result.begin(), result.end());
    return result;
}

template<size_t TBits, bool TSigned>
constexpr void FixedInt<TBits, TSigned>::ClearUnusedBits() {
    if constexpr(TBits % 32 != 0) {
        m_Limbs.back() &= (1u << (TBits % 32)) - 1;
    }
}

template<size_t TBits, bool TSigned>
inline constexpr FixedInt<TBits, TSigned> FixedInt<TBits, TSigned>::MinValue = [] {
    FixedInt<TBits, TSigned> result;
    if constexpr(TSigned) {
        result.m_Limbs.back() = SignMask;
    }
    return result;
}();

template<size_t TBits, bool TSigned>
inline constexpr FixedInt<TBits, TSigned> FixedInt<TBits, TSigned>::MaxValue = [] { 
    FixedInt<TBits, TSigned> result; 
    result.m_Limbs.fill(std::numeric_limits<u32>::max());
    if constexpr(TBits % 32 != 0) {
        result.m_Limbs.back() &= (1u << (TBits % 32)) - 1;
    }
    if constexpr(TSigned) {
        result.m_Limbs.back() &= ~SignMask;
    }

    return result;
}();