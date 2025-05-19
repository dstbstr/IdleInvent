#pragma once

#include <imgui.h>

ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs);
ImVec2 operator/(const ImVec2& lhs, float divisor);

void TextCenteredX(const char* text);
void TextCenterdY(const char* text);
void TextCentered(const char* text);
