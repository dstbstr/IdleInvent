#include "Ui/Panel/Panel.h"
#include "Ui/Panel/Connection.h"

#include <imgui.h>
#include <algorithm>
#include <cmath>
#include <format>

namespace Ui {
    ImVec2 ToScreenSpace(const ImVec2& localPos) {
        const auto screenCursor = ImGui::GetCursorScreenPos();
        const auto localCursor = ImGui::GetCursorPos();
        const ImVec2 origin{screenCursor.x - localCursor.x, screenCursor.y - localCursor.y};
        return {origin.x + localPos.x, origin.y + localPos.y};
    }

    UiRect ToScreenSpace(const UiRect& localRect) {
        auto screenCursor = ImGui::GetCursorScreenPos();
        auto localCursor = ImGui::GetCursorPos();
        auto origin = ImVec2{
            screenCursor.x - localCursor.x, 
            screenCursor.y - localCursor.y
        };

        return {
            ImVec2{
                origin.x + localRect.Min.x, 
                origin.y + localRect.Min.y}, 
            ImVec2 {
                origin.x + localRect.Max.x,
                origin.y + localRect.Max.y
                }
            };
    }

    void DrawLine(const Connection& connection) {
        auto* drawList = ImGui::GetWindowDrawList();
        const auto from = ToScreenSpace(connection.From);
        const auto to = ToScreenSpace(connection.To);

        drawList->AddLine(from, to, connection.Color, connection.Thickness);
    }

    void DrawCorner(const Connection& connection) {
        auto* drawList = ImGui::GetWindowDrawList();
        const auto from = ToScreenSpace(connection.From);
        const auto to = ToScreenSpace(connection.To);
        const auto dx = std::abs(to.x - from.x);
        const auto dy = std::abs(to.y - from.y);

        // Travel along the main axis first, then bend once toward the destination on the cross axis.
        const auto corner = (dy >= dx)
            ? ImVec2{from.x, to.y}
            : ImVec2{to.x, from.y};

        drawList->AddLine(from, corner, connection.Color, connection.Thickness);
        drawList->AddLine(corner, to, connection.Color, connection.Thickness);
    }

    void Panel::Render() {
        UpdatePan();
        UpdateZoom();
        RenderBackground();

        ImGui::SetCursorPos(Bounds.Min);
        ImGui::BeginChild(
            ImGui::GetID(static_cast<const void*>(this)),
            Bounds.GetSize(),
            0,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
        );
        RenderImpl();
        ImGui::EndChild();
    }

    void Panel::UpdatePan() {
        auto bounds = ToScreenSpace(Bounds);

        if(ImGui::IsMouseHoveringRect(bounds.Min, bounds.Max, false) && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.f)) {
            const auto mouseDelta = ImGui::GetIO().MouseDelta;
            PanOffset.x += mouseDelta.x;
            PanOffset.y += mouseDelta.y;
        }
    }

    void Panel::ZoomAt(s32 delta, ImVec2 fixedPointPanelLocal) {
        if(!ZoomFn || delta == 0) return;

        const auto oldLevel = ZoomLevel;
        const auto oldZoom = ZoomFn(oldLevel);

        ZoomLevel += delta;
        const auto newZoom = ZoomFn(ZoomLevel);

        // ZoomFn clamps levels inside its own bounds, so when the factor doesn't change we
        // know we hit the limit -- revert so ZoomLevel doesn't drift past where it can do work.
        if(newZoom == oldZoom) {
            ZoomLevel = oldLevel;
            return;
        }
        if(oldZoom == 0.f) return;

        const auto r = newZoom / oldZoom;
        PanOffset.x = (fixedPointPanelLocal.x - ContentOrigin.x) * (1.f - r) + PanOffset.x * r;
        PanOffset.y = (fixedPointPanelLocal.y - ContentOrigin.y) * (1.f - r) + PanOffset.y * r;
    }

    void Panel::ZoomIn() {
        ZoomAt(+1, {Bounds.GetWidth() * 0.5f, Bounds.GetHeight() * 0.5f});
    }

    void Panel::ZoomOut() {
        ZoomAt(-1, {Bounds.GetWidth() * 0.5f, Bounds.GetHeight() * 0.5f});
    }

    void Panel::UpdateZoom() {
        if(!ZoomFn) return;

        const auto bounds = ToScreenSpace(Bounds);
        if(!ImGui::IsMouseHoveringRect(bounds.Min, bounds.Max, false)) return;

        const auto wheel = ImGui::GetIO().MouseWheel;
        if(wheel == 0.f) return;

        const auto mouseScreen = ImGui::GetIO().MousePos;
        const ImVec2 mousePanelLocal{
            mouseScreen.x - bounds.Min.x,
            mouseScreen.y - bounds.Min.y
        };

        const s32 delta = wheel > 0.f ? +1 : -1;
        ZoomAt(delta, mousePanelLocal);
    }

    void Panel::RenderBackground() const {
        const auto bounds = ToScreenSpace(Bounds);
        const auto panelMin = bounds.Min;
        const auto panelMax = bounds.Max;

        if(BackgroundColor) {
            ImGui::GetWindowDrawList()->AddRectFilled(panelMin, panelMax, *BackgroundColor);
        }
        if(BackgroundTexture) {
            ImGui::GetWindowDrawList()->AddImage(*BackgroundTexture, panelMin, panelMax);
        }
    }

}