#pragma once

#include <Platform/NumTypes.h>

#include <string>

struct Modifier;

struct ModifierSave {
    u16 Add{0};
    u16 Mul{0}; // fixed point, max value = 65535 = 65.535

    void Load(Modifier& outModifier) const;
};

struct Modifier {
    s64 Add{0};
    f32 Mul{1.0f};

    constexpr bool operator==(const Modifier& other) const {
		return Add == other.Add && Mul == other.Mul;
	}
    constexpr bool operator!=(const Modifier& other) const { return !(*this == other); }

    constexpr Modifier& operator+=(const Modifier& other) {
		Add += other.Add;
		Mul *= other.Mul;
		return *this;
	}

    friend constexpr Modifier operator+(const Modifier& lhs, const Modifier& rhs) {
        return {lhs.Add + rhs.Add, lhs.Mul * rhs.Mul};
    }

    constexpr auto Apply(auto value) const {
        return static_cast<decltype(value)>(static_cast<f32>(static_cast<s64>(value) + Add) * Mul);
    }

    constexpr f32 GetBonus() const { return static_cast<f32>(Add) * Mul; }

    void Save(ModifierSave& outSave) const;
};

std::string ToString(const Modifier& mod);
