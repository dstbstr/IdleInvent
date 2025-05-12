#pragma once

#include <imgui.h>

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
ImVec2 operator/(const ImVec2& lhs, float divisor) {
    return {lhs.x / divisor, lhs.y / divisor};
}

void TextCenteredX(const char* text) {
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(text).x) / 2);
    ImGui::Text("%s", text);
}
void TextCenterdY(const char* text) {
    ImGui::SetCursorPosY((ImGui::GetWindowSize().y - ImGui::CalcTextSize(text).y) / 2);
    ImGui::Text("%s", text);
}
void TextCentered(const char* text) {
    auto size = ImGui::CalcTextSize(text);
    auto winPos = ImGui::GetWindowSize();
    auto finalPos = (winPos - size) / 2;
    ImGui::SetCursorPos(finalPos);
    ImGui::Text("%s", text);
}
