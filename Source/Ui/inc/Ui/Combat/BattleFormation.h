#pragma once

#include "Platform/NumTypes.h"
#include "Ui/UiGeometry.h"

#include <imgui.h>
#include <cstddef>
#include <span>
#include <vector>

namespace Ui::Formation {
	enum struct Front : u8 {
        Left, Right, Top, Bottom
	};

	struct Slot {
        ImVec2 Center{};
        f32 AvailableSize{};
	};

	[[nodiscard]] std::vector<Slot> Square(size_t count, UiRect region, Front front);
    [[nodiscard]] std::vector<Slot> Triangle(size_t count, UiRect region, Front front);
    [[nodiscard]] std::vector<Slot> Lines(std::span<const size_t> lineCounts, UiRect region, Front front);
}