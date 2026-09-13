#include "Ui/UiUtil.h"
#include "Ui/UiGeometry.h"
#include "Platform/Graphics.h"
#include "imgui.h"

namespace {
    bool fontsInitialized = false;
}

void TextCenteredX(const char* text) {
    auto origin = ImGui::GetCursorPosX();
    auto available = ImGui::GetContentRegionAvail().x;
    auto size = ImGui::CalcTextSize(text).x;
    ImGui::SetCursorPosX(origin + (available - size) * 0.5f);
    ImGui::TextUnformatted(text);
}

void TextCenterdY(const char* text) {
    auto origin = ImGui::GetCursorPosY();
    auto available = ImGui::GetContentRegionAvail().y;
    auto size = ImGui::CalcTextSize(text).y;
    ImGui::SetCursorPosY(origin + (available - size) * 0.5f);
    ImGui::TextUnformatted(text);
}

void TextCentered(const char* text) {
    auto origin = ImGui::GetCursorPos();
    auto available = ImGui::GetContentRegionAvail();
    auto size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPos(origin + (available - size) * 0.5f);
    ImGui::TextUnformatted(text);
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

void DrawSprite(const Sprite& sprite, ImVec2 imageSize) {
    ImGui::Image(sprite.Texture, imageSize, sprite.UvMin, sprite.UvMax);
}

bool SpriteButton(const char* label, const Sprite& sprite, ImVec2 imageSize, std::optional<ButtonColors> buttonColors) {
    if(buttonColors) {
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColors->BackgroundColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColors->HoveredColor);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColors->ActiveColor);
    }
    auto clicked = ImGui::ImageButton(label, sprite.Texture, imageSize, sprite.UvMin, sprite.UvMax);
    if(buttonColors) ImGui::PopStyleColor(3);

    return clicked;
}