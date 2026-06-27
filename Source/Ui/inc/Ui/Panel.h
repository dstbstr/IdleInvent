#pragma once
#include "Platform/NumTypes.h"
#include "Ui/Rect.h"
#include <imgui.h>
#include <optional>

namespace Ui {
    struct Connection;

    struct PanelConfig {
        Rect Bounds{};
        std::optional<ImU32> BackgroundColor{std::nullopt};
        std::optional<ImTextureID> BackgroundTexture{std::nullopt};        
    };

    class Panel {
    private:
        Rect Bounds{};
        ImVec2 PanOffset{};
        std::optional<ImU32> BackgroundColor{std::nullopt};
        std::optional<ImTextureID> BackgroundTexture{std::nullopt};
        
    protected:
        ImVec2 GetOrigin() const { 
            return {Bounds.Pos.x + PanOffset.x, Bounds.Pos.y + PanOffset.y}; 
        }
        f32 GetCenterX() const { return Bounds.Size.x * 0.5f; }
        f32 GetCenterY() const { return Bounds.Size.y * 0.5f; }

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
            , BackgroundTexture(config.BackgroundTexture) {}

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

    protected: 
        virtual void RenderImpl() = 0;

    private:
        void UpdatePan();
        void RenderBackground() const;
    };

    ImVec2 ToScreenSpace(const ImVec2& localPos);
    Rect ToScreenSpace(const Rect& localRect);
    void DrawLine(const Connection& connection);
    void DrawCorner(const Connection& connection);

}
