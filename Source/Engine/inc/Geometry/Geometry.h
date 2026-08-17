#pragma once

#include "Constexpr/ConstexprMath.h"

#include <compare>

namespace Geometry {
	template<typename T, typename TSpace>
	struct Pos2 {
        T X{};
        T Y{};

		constexpr auto operator<=>(const Pos2&) const = default;

        constexpr Pos2 operator-(const Pos2& rhs) const { return {X - rhs.X, Y - rhs.Y}; }
        constexpr Pos2& operator-=(const Pos2& rhs) {
            X -= rhs.X;
            Y -= rhs.Y;
            return *this;
        }

        constexpr Pos2 operator*(T scalar) const { return {X * scalar, Y * scalar}; }
        constexpr Pos2& operator*=(T scalar) { 
            X *= scalar;
            Y *= scalar;
            return *this;
        }

        constexpr T MDistance(const Pos2& rhs) const {
            // fix this if overflow is a concern
            return static_cast<T>(Constexpr::Abs(X - rhs.X) + Constexpr::Abs(Y - rhs.Y));
        }
	};

    template<typename T, typename TSpace>
    constexpr T MDistance(Pos2<T, TSpace> a, Pos2<T, TSpace> b) {
        return a.MDistance(b);
    }

    template<typename T, typename TSpace>
    struct Size2 {
        T X{};
        T Y{};

        constexpr auto operator<=>(const Size2&) const = default;
        constexpr Size2 operator+(const Size2& rhs) const { return {X + rhs.X, Y + rhs.Y}; }
        constexpr Size2& operator+=(const Size2& rhs) {
            X += rhs.X;
            Y += rhs.Y;
            return *this;
        }

        constexpr Size2 operator-(const Size2& rhs) const { return {X - rhs.X, Y - rhs.Y}; }
        constexpr Size2& operator-=(const Size2& rhs) {
            X -= rhs.X;
            Y -= rhs.Y;
            return *this;
        }

        constexpr Size2 operator*(T scalar) const { return {X * scalar, Y * scalar}; }
        constexpr Size2& operator*=(T scalar) {
            X *= scalar;
            Y *= scalar;
            return *this;
        }
    };

    template<typename T, typename TSpace>
    constexpr Pos2<T, TSpace> operator+(Pos2<T, TSpace> pos, Size2<T, TSpace> size) {
        return {pos.X + size.X, pos.Y + size.Y};
    }

    template<typename T, typename TSpace>
    constexpr Pos2<T, TSpace> operator-(Pos2<T, TSpace> pos, Size2<T, TSpace> size) {
        return {pos.X - size.X, pos.Y - size.Y};
    }

	template<typename T, typename TSpace>
	struct Rect {
        using TPos = Pos2<T, TSpace>;
        using TSize = Size2<T, TSpace>;

        TPos Pos{};
        TSize Size{};

        constexpr Rect() = default;
        constexpr Rect(TPos pos, TSize size) : Pos(pos), Size(size) {}
        constexpr Rect(TPos tl, TPos br) : Pos(tl), Size({br.X - tl.X, br.Y - tl.Y}) {}

        constexpr TPos Tl() const { return Pos; }
        constexpr TPos Br() const { return Pos + Size; }

        constexpr Rect Expand(TSize delta = {1, 1}) const { 
            return {Pos - delta, Size + delta * 2};
        }
        constexpr Rect Contract(TSize delta = {1, 1}) const {
            return {Pos + delta, Size - delta * 2};
        }
	};
}