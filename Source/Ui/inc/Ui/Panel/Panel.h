#pragma once
#include "Platform/NumTypes.h"
#include "Ui/Rect.h"
#include <imgui.h>
#include <functional>
#include <optional>

namespace Ui {
    struct Connection;

    enum struct Anchor : u8 {
        TopLeft, TopCenter, TopRight,
        LeftCenter, Center, RightCenter,
        BottomLeft, BottomCenter, BottomRight
    };

    struct PanelConfig {
        Rect Bounds{};
        std::optional<ImU32> BackgroundColor{std::nullopt};
        std::optional<ImTextureID> BackgroundTexture{std::nullopt};
        std::function<f32(s32)> ZoomFn{};
    };

    class Panel {
    private:
        Rect Bounds{};
        ImVec2 PanOffset{};
        std::optional<ImU32> BackgroundColor{std::nullopt};
        std::optional<ImTextureID> BackgroundTexture{std::nullopt};
        s32 ZoomLevel{0};
        std::function<f32(s32)> ZoomFn{};
        ImVec2 ContentOrigin{};

    protected:
        ImVec2 GetOrigin() const { 
            return {Bounds.Pos.x + PanOffset.x, Bounds.Pos.y + PanOffset.y}; 
        }
        f32 GetCenterX() const { return Bounds.Size.x * 0.5f; }
        f32 GetCenterY() const { return Bounds.Size.y * 0.5f; }
        ImVec2 GetSize() const { return Bounds.Size; }
        ImVec2 GetAnchor(Anchor anchor) const {
            switch(anchor) {
                using enum Anchor;
            case TopLeft:      return {0.f, 0.f};
            case TopCenter:    return {GetCenterX(), 0.f};
            case TopRight:     return {Bounds.Size.x, 0.f};
            case LeftCenter:   return {0.f, GetCenterY()};
            case Center:       return {GetCenterX(), GetCenterY()};
            case RightCenter:  return {Bounds.Size.x, GetCenterY()};
            case BottomLeft:   return {0.f, Bounds.Size.y};
            case BottomCenter: return {GetCenterX(), Bounds.Size.y};
            case BottomRight:  return {Bounds.Size.x, Bounds.Size.y};
            }
            return {};
        }

    public:
        Panel(
            Rect bounds,
            std::optional<ImU32> backgroundColor = std::nullopt,
            std::optional<ImTextureID> backgroundTexture = std::nullopt)
            : Bounds(bounds)
            , BackgroundColor(backgroundColor)
            , BackgroundTexture(backgroundTexture) {}

        Panel(const PanelConfig& config)
            : Bounds(config.Bounds)
            , PanOffset({0.f, 0.f})
            , BackgroundColor(config.BackgroundColor)
            , BackgroundTexture(config.BackgroundTexture)
            , ZoomFn(config.ZoomFn) {}

        virtual ~Panel() = default;
        Panel(const Panel&) = delete;
        Panel& operator=(const Panel&) = delete;
        Panel(Panel&&) = default;
        Panel& operator=(Panel&&) = default;

        void Render();

        void SetBounds(Rect bounds) {
            Bounds = bounds;
        }

        ImVec2 GetPanOffset() const { return PanOffset; }
        void ResetPan() { PanOffset = {0.f, 0.f}; }

        // Zoom is opt-in: pass a ZoomFn in PanelConfig to enable. GetZoom() returns 1.f when disabled.
        bool ZoomEnabled() const { return static_cast<bool>(ZoomFn); }
        f32 GetZoom() const { return ZoomFn ? ZoomFn(ZoomLevel) : 1.f; }
        void ResetZoom() { ZoomLevel = 0; }

        // Panel-local point where the consumer's content layout origin sits (e.g., the
        // TreePanel anchor). Used by zoom-around-point math so wheel zoom keeps the cursor
        // over the same content. Consumers should call this each frame before Render if it
        // changes; default is (0, 0).
        void SetContentOrigin(ImVec2 origin) { ContentOrigin = origin; }

        // Change zoom by delta levels, adjusting PanOffset so that fixedPointPanelLocal
        // (a point in panel-local coords) stays over the same content.
        void ZoomAt(s32 delta, ImVec2 fixedPointPanelLocal);

        // Convenience: zoom around the panel center. Intended for button-driven UI.
        void ZoomIn();
        void ZoomOut();

    protected: 
        virtual void RenderImpl() = 0;

    private:
        void UpdatePan();
        void UpdateZoom();
        void RenderBackground() const;
    };

    ImVec2 ToScreenSpace(const ImVec2& localPos);
    Rect ToScreenSpace(const Rect& localRect);
    void DrawLine(const Connection& connection);
    void DrawCorner(const Connection& connection);

}
