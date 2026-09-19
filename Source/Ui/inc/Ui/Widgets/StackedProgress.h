#pragma once

#include <Platform/NumTypes.h>
#include <imgui.h>
#include <span>

namespace Ui {
	struct ProgressSegment {
        f32 Fraction{};
        ImU32 Color{};
	};

	void MultiProgress(std::span<const ProgressSegment> segments);
}