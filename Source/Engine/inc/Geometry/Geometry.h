#pragma once

#include "Platform/NumTypes.h"
#include "Constexpr/ConstexprMath.h"

#include <algorithm>
#include <compare>
#include <type_traits>

namespace Geometry {
    struct Scale2 {
        f32 X{1.f};
        f32 Y{1.f};

        constexpr auto operator<=>(const Scale2&) const = default;

        [[nodiscard]] static constexpr Scale2 Uniform(f32 value) { return {value, value}; }
        [[nodiscard]] static constexpr Scale2 Half() { return {0.5f, 0.5f}; }
    };

	template<typename T, typename TSpace>
	struct Point2 {
        T X{};
        T Y{};

		constexpr auto operator<=>(const Point2&) const = default;

        constexpr Point2 operator-(const Point2& rhs) const { return {X - rhs.X, Y - rhs.Y}; }
        constexpr Point2& operator-=(const Point2& rhs) {
            X -= rhs.X;
            Y -= rhs.Y;
            return *this;
        }

        constexpr Point2 operator*(T scalar) const { return {X * scalar, Y * scalar}; }
        constexpr Point2& operator*=(T scalar) { 
            X *= scalar;
            Y *= scalar;
            return *this;
        }

        constexpr T MDistance(const Point2& rhs) const {
            // fix this if overflow is a concern
            return static_cast<T>(Constexpr::Abs(X - rhs.X) + Constexpr::Abs(Y - rhs.Y));
        }
	};

    template<typename T, typename TSpace>
    constexpr T MDistance(Point2<T, TSpace> a, Point2<T, TSpace> b) {
        return a.MDistance(b);
    }

    template<typename T, typename TSpace>
    struct Size2 {
        T X{};
        T Y{};

        [[nodiscard]] static constexpr Size2 One() { return {T{1}, T{1}}; }
        [[nodiscard]] static constexpr Size2 Zero() { return {T{0}, T{0}}; }

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

        constexpr Size2 operator/(T scalar) const { return {X / scalar, Y / scalar}; }
        constexpr Size2& operator/=(T scalar) {
            X /= scalar;
            Y /= scalar;
            return *this;
        }
    };

    template<typename T, typename TSpace>
    constexpr Point2<T, TSpace> operator+(Point2<T, TSpace> pos, Size2<T, TSpace> size) {
        return {pos.X + size.X, pos.Y + size.Y};
    }

    template<typename T, typename TSpace>
    constexpr Point2<T, TSpace> operator-(Point2<T, TSpace> pos, Size2<T, TSpace> size) {
        return {pos.X - size.X, pos.Y - size.Y};
    }

	template<typename T, typename TSpace>
	struct Rect {
        using TPos = Point2<T, TSpace>;
        using TSize = Size2<T, TSpace>;

        TPos Pos{};
        TSize Size{};

        constexpr Rect() = default;
        constexpr Rect(TPos pos, TSize size) : Pos(pos), Size(size) {}
        constexpr Rect(TPos tl, TPos br) : Pos(tl), Size({br.X - tl.X, br.Y - tl.Y}) {}
        static constexpr Rect FromCenterSize(TPos center, TSize size) {
            return {center - size / 2, size};
        }

        [[nodiscard]] constexpr TPos Tl() const { return Pos; }
        [[nodiscard]] constexpr TPos Br() const { return Pos + Size; }
        [[nodiscard]] constexpr TPos Center() const { return Pos + Size / 2; }

        constexpr Rect Expand(TSize delta = {1, 1}) const { 
            return {Pos - delta, Size + delta * 2};
        }
        constexpr Rect Contract(TSize delta = {1, 1}) const {
            return {Pos + delta, Size - delta * 2};
        }
	};

    template<typename TValue, typename TSpace>
    [[nodiscard]] constexpr auto operator*(Size2<TValue, TSpace> size, Scale2 scale) {
        using TResult = std::common_type_t<TValue, f32>;

        return Size2<TResult, TSpace>{
            static_cast<TResult>(size.X) * static_cast<TResult>(scale.X),
            static_cast<TResult>(size.Y) * static_cast<TResult>(scale.Y)
        };
    }

    template<typename TValue, typename TSpace>
    [[nodiscard]] constexpr auto operator*(Point2<TValue, TSpace> point, Scale2 scale) {
        using TResult = std::common_type_t<TValue, f32>;
        return Point2<TResult, TSpace>{
            static_cast<TResult>(point.X) * static_cast<TResult>(scale.X),
            static_cast<TResult>(point.Y) * static_cast<TResult>(scale.Y)
        };
    }

    template<typename TValue, typename TSpace>
    [[nodiscard]] constexpr auto operator*(Rect<TValue, TSpace> rect, Scale2 scale) {
        using TResult = std::common_type_t<TValue, f32>;
        return Rect<TResult, TSpace>{rect.Pos * scale, rect.Size * scale};
    }

    template<typename TResult, typename TValue, typename TSpace>
    [[nodiscard]] constexpr Size2<TResult, TSpace> SizeCast(Size2<TValue, TSpace> size) {
        return {static_cast<TResult>(size.X), static_cast<TResult>(size.Y)};
    }

    template<typename T, typename TSpace>
    [[nodiscard]] constexpr Size2<T, TSpace> Min(Size2<T, TSpace> lhs, Size2<T, TSpace> rhs) {
        return {std::min(lhs.X, rhs.X), std::min(lhs.Y, rhs.Y)};
    }
    template<typename T, typename TSpace>
    [[nodiscard]] constexpr Size2<T, TSpace> Max(Size2<T, TSpace> lhs, Size2<T, TSpace> rhs) {
        return {std::max(lhs.X, rhs.X), std::max(lhs.Y, rhs.Y)};
    }

}