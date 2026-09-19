#include "Ui/Widgets/StackedProgress.h"

#include <algorithm>
#include <cmath>

namespace Ui {
	void MultiProgress(std::span<const ProgressSegment> segments) {
        auto origin = ImGui::GetCursorScreenPos();
        auto width = ImGui::GetContentRegionAvail().x;
        auto height = ImGui::GetFrameHeight();
        ImGui::Dummy({width, height});
        if(width == 0.f) return;

        auto end = ImVec2{origin.x + width, origin.y + height};
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(origin, end, IM_COL32_BLACK);

        auto filled = 0.f;
        for(const auto& segment : segments) {
            auto [frac, color] = segment;
            if(!std::isfinite(frac) || frac <= 0.f) continue;

            auto next = filled + std::min(frac, 1.f - filled);
            drawList->AddRectFilled(
                {origin.x + width * filled, origin.y}, 
                {origin.x + width * next, end.y}, color
            );
            filled = next;
            if(filled >= 1.f) break;
        }

        drawList->AddRect(origin, end, ImGui::GetColorU32(ImGuiCol_Border));
	}
}