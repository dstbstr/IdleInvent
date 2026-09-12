#pragma once

#include <Platform/NumTypes.h>
#include <Platform/Graphics.h>

#include <imgui.h>
#include <optional>
#include <string>

static constexpr auto BaseUiFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
static constexpr auto ImageFlags = BaseUiFlags | ImGuiWindowFlags_NoInputs;

struct ButtonColors {
    ImU32 BackgroundColor{};
    ImU32 HoveredColor{};
    ImU32 ActiveColor{};
};
enum struct FontSizes : u8 { H1, H2, H3, H4 };
void InitializeFonts(const std::string& fontName = "DroidSans.ttf");
ImFont* GetFont(FontSizes font);

void TextCenteredX(const char* text);
void TextCenterdY(const char* text);
void TextCentered(const char* text);

void DrawSprite(const Sprite& sprite, ImVec2 imageSize);
bool SpriteButton(const char* label, const Sprite& sprite, ImVec2 imageSize, std::optional<ButtonColors> buttonColors = std::nullopt );
