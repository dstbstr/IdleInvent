#pragma once
#include <Geometry/Geometry.h>
#include <World/World.h>

#include <imgui.h>

namespace Ui {
    struct MapContentSpace;
    using ContentPoint = Geometry::Point2<f32, MapContentSpace>;
    using ContentSize = Geometry::Size2<f32, MapContentSpace>;
    using ContentRect = Geometry::Rect<f32, MapContentSpace>;

	template<typename T, typename TSpace>
	constexpr ImVec2 ToUi(const Geometry::Point2<T, TSpace>& point) {
        return ImVec2(static_cast<f32>(point.X), static_cast<f32>(point.Y));
	}
	template<typename T, typename TSpace>
    constexpr ImVec2 ToUi(const Geometry::Size2<T, TSpace>& size) {
        return ImVec2(static_cast<f32>(size.X), static_cast<f32>(size.Y));
    }

	template<typename T, typename TSpace>
    constexpr Geometry::Point2<T, TSpace> ToPoint(const ImVec2& point) {
        return Geometry::Point2<T, TSpace>{static_cast<T>(point.x), static_cast<T>(point.y)};
    }
    template<typename T, typename TSpace>
    constexpr Geometry::Size2<T, TSpace> ToSize(const ImVec2& size) {
        return Geometry::Size2<T, TSpace>{static_cast<T>(size.x), static_cast<T>(size.y)};
    }

    struct UiRect {
        ImVec2 Min{};
        ImVec2 Max{};

        static constexpr UiRect FromPosSize(const ImVec2& point, const ImVec2& size) {
            return UiRect{.Min = point, .Max = {point.x + size.x, point.y + size.y}};
        }

        [[nodiscard]] constexpr f32 GetWidth() const { return Max.x - Min.x; }
        [[nodiscard]] constexpr f32 GetHeight() const { return Max.y - Min.y; }
        [[nodiscard]] constexpr ImVec2 GetSize() const { return {GetWidth(), GetHeight()}; }

        [[nodiscard]] constexpr f32 CenterX() const { return Min.x + GetWidth() * 0.5f; }
        [[nodiscard]] constexpr f32 CenterY() const { return Min.y + GetHeight() * 0.5f; }
        [[nodiscard]] constexpr ImVec2 GetCenter() const { return {CenterX(), CenterY()}; }
    };

    template<typename T, typename TSpace>
    constexpr UiRect ToUi(const Geometry::Rect<T, TSpace>& rect) {
        return UiRect{ToUi(rect.Tl()), ToUi(rect.Br())};
    }
    template<typename T, typename TSpace>
    constexpr Geometry::Rect<T, TSpace> ToRect(const UiRect& rect) {
        return Geometry::Rect<T, TSpace>{ToPoint<T, TSpace>(rect.Min), ToPoint<T, TSpace>(rect.Max)};
    }

    template<f32 TCellSize>
    constexpr ContentRect ToContentRect(World::CellCoord cell) {
        return {
            ContentPoint{static_cast<f32>(cell.X) * TCellSize, static_cast<f32>(cell.Y) * TCellSize},
            ContentSize{TCellSize, TCellSize}
        };
    }

    constexpr UiRect ToUi(const ContentRect& rect) { 
        return UiRect{
            ToUi(rect.Tl()), 
            ToUi(rect.Br())
        }; 
    }
}