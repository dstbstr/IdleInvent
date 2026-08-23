#include "Ui/UiUtil.h"
#include "Ui/UiGeometry.h"
#include "Platform/Graphics.h"
#include "imgui.h"

namespace {
    bool fontsInitialized = false;
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

void InitializeFonts(const std::string& fontName) {
    if(fontsInitialized) return;
    auto* font = fontName.c_str();
    Graphics::TryLoadFont("H1", font, 32.0f);
    Graphics::TryLoadFont("H2", font, 24.0f);
    Graphics::TryLoadFont("H3", font, 16.0f);
    Graphics::TryLoadFont("H4", font, 12.0f);
    fontsInitialized = true;
}

ImFont* GetFont(FontSizes fontSize) {
    if(!fontsInitialized) InitializeFonts();
    switch(fontSize) {
        using enum FontSizes;
        case H1: return Graphics::GetFont("H1");
        case H2: return Graphics::GetFont("H2");
        case H3: return Graphics::GetFont("H3");
        case H4: return Graphics::GetFont("H4");
        default: return nullptr;
    }
}
