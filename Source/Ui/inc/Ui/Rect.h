#pragma once

#include <imgui.h>

namespace Ui {
	struct Rect {
        ImVec2 Pos{0.f, 0.f};
        ImVec2 Size{0.f, 0.f};

		constexpr ImVec2 GetBottomRight() const { return {Pos.x + Size.x, Pos.y + Size.y}; }
	};
}