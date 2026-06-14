#pragma once

#include "Platform/NumTypes.h"
#include <imgui.h>
#include <string>

static constexpr auto BaseUiFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
static constexpr auto ImageFlags = BaseUiFlags | ImGuiWindowFlags_NoInputs;

enum struct FontSizes : u8 { H1, H2, H3, H4 };
void InitializeFonts(const std::string& fontName = "DroidSans.ttf");
ImFont* GetFont(FontSizes font);

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator/(const ImVec2& lhs, float divisor);

void TextCenteredX(const char* text);
void TextCenterdY(const char* text);
void TextCentered(const char* text);
