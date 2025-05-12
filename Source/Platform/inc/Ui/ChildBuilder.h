#pragma once

#include "Instrumentation/Logging.h"

#include <imgui.h>
#include <optional>

class ChildWin {
public:
    ~ChildWin() {
        if(m_Font) {
            ImGui::PopFont();
        }
        ImGui::EndChild();
        if(m_BgColor) {
            ImGui::PopStyleColor();
        }
    }

private:
    friend class ChildBuilder;
    ChildWin(const char* label, ImVec2 dims, ImFont* font, std::optional<ImVec4> bg, bool centerX, bool centerY) : m_Font(font), m_BgColor(bg) {
        if(m_BgColor) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, *m_BgColor);
        }
        ImGui::BeginChild(label, dims);
        if(m_Font) {
            ImGui::PushFont(m_Font);
        }
        if(centerX) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + dims.x / 2);
        }
        if(centerY) {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + dims.y / 2);
        }
    }
    ImFont* m_Font{nullptr};
    std::optional<ImVec4> m_BgColor{};
    bool m_CenterX{false};
    bool m_CenterY{false};
};
class ChildBuilder {
public:
    ChildBuilder(const char* label, ImVec2 dims) : m_Label(label), m_Dims(dims) {}

    ChildBuilder(const char* label, float xAndY) : ChildBuilder(label, ImVec2(xAndY, xAndY)) {}

    ChildBuilder& WithFont(ImFont* font) {
        DR_ASSERT(m_Font == nullptr);
        m_Font = font;
        return *this;
    }

    ChildBuilder& WithBgColor(ImVec4 color) {
        DR_ASSERT(!m_BgColor.has_value());
        m_BgColor = color;
        return *this;
    }

    ChildBuilder& CenterX() {
        m_CenterX = true;
        return *this;
    }

    ChildBuilder& CenterY() {
        m_CenterY = true;
        return *this;
    }
    [[nodiscard]] ChildWin Build() { return {m_Label, m_Dims, m_Font, m_BgColor, m_CenterX, m_CenterY}; }

private:
    const char* m_Label;
    ImVec2 m_Dims;
    ImFont* m_Font{nullptr};
    std::optional<ImVec4> m_BgColor{};
    bool m_CenterX {false};
    bool m_CenterY {false};
};