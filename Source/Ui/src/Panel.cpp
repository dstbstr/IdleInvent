#include "Ui/Panel.h"
#include "Ui/Connection.h"

#include <imgui.h>
#include <cmath>
#include <format>

namespace Ui {
    ImVec2 ToScreenSpace(const ImVec2& localPos) {
        const auto screenCursor = ImGui::GetCursorScreenPos();
        const auto localCursor = ImGui::GetCursorPos();
        const ImVec2 origin{screenCursor.x - localCursor.x, screenCursor.y - localCursor.y};
        return {origin.x + localPos.x, origin.y + localPos.y};
    }

    Rect ToScreenSpace(const Rect& localRect) {
        const auto screenCursor = ImGui::GetCursorScreenPos();
        const auto localCursor = ImGui::GetCursorPos();
        const ImVec2 origin{screenCursor.x - localCursor.x, screenCursor.y - localCursor.y};
        return {ImVec2{origin.x + localRect.Pos.x, origin.y + localRect.Pos.y}, localRect.Size};
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

        ImVec2 p1, p2;
        if(dy >= dx) {
            const auto mid = (from.y + to.y) / 2.f;
            p1 = ImVec2(from.x, mid);
            p2 = ImVec2(to.x, mid);
        } else {
            const auto mid = (from.x + to.x) / 2.f;
            p1 = ImVec2(mid, from.y);
            p2 = ImVec2(mid, to.y);
        }

        drawList->AddLine(from, p1, connection.Color, connection.Thickness);
        drawList->AddLine(p1, p2, connection.Color, connection.Thickness);
        drawList->AddLine(p2, to, connection.Color, connection.Thickness);
    }

    void Panel::Render() {
        UpdatePan();
        RenderBackground();

        auto bounds = ToScreenSpace(Bounds);
        auto min = bounds.Pos;
        auto max = bounds.GetBottomRight();

        ImGui::PushClipRect(min, max, true);
        RenderImpl();
        ImGui::PopClipRect();
    }

    void Panel::UpdatePan() {
        auto bounds = ToScreenSpace(Bounds);
        auto min = bounds.Pos;
        auto max = bounds.GetBottomRight();

        if(ImGui::IsMouseHoveringRect(min, max, false) && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.f)) {
            const auto mouseDelta = ImGui::GetIO().MouseDelta;
            PanOffset.x += mouseDelta.x;
            PanOffset.y += mouseDelta.y;
        }
    }

    void Panel::RenderBackground() const {
        const auto bounds = ToScreenSpace(Bounds);
        const auto panelMin = bounds.Pos;
        const auto panelMax = bounds.GetBottomRight();

        if(BackgroundColor) {
            ImGui::GetWindowDrawList()->AddRectFilled(panelMin, panelMax, *BackgroundColor);
        }
        if(BackgroundTexture) {
            ImGui::GetWindowDrawList()->AddImage(*BackgroundTexture, panelMin, panelMax);
        }
    }

}