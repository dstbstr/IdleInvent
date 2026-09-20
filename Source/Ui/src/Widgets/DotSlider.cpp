#include "Ui/Widgets/DotSlider.h"

#include <algorithm>

namespace Ui {
    void DotSlider(const char* id, f32& position, const char* leftLabel, const char* rightLabel, size_t tickCount, f32 minPos, f32 maxPos) {
        position = std::clamp(position, minPos, maxPos);
        auto origin = ImGui::GetCursorScreenPos();
        auto width = ImGui::GetContentRegionAvail().x;
        auto trackHeight = ImGui::GetFrameHeight();
        auto radius = ImGui::GetFontSize() * 0.15f;
        if(width <= radius * 2.f) return;

        auto gap = ImGui::GetStyle().ItemSpacing.y;
        auto hasLabels = leftLabel[0] != '\0' || rightLabel[0] != '\0';

        auto height = trackHeight + (hasLabels ? gap + ImGui::GetTextLineHeight() : 0.f);

        auto start = ImVec2{origin.x + radius, origin.y + trackHeight * 0.5f};
        auto end = ImVec2{origin.x + width - radius, start.y};
        auto trackWidth = end.x - start.x;
        ImGui::InvisibleButton(id, {width, height}); 
        if(ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            position = std::clamp((ImGui::GetMousePos().x - start.x) / trackWidth, minPos, maxPos);
        }

        auto trackColor = ImGui::GetColorU32(ImGuiCol_Separator);
        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddLine(start, end, trackColor, 2.f);

        for(size_t i = 0; i < tickCount; i++) {
            auto frac = (static_cast<f32>(i) + 1.f) / (static_cast<f32>(tickCount) + 1.f);
            auto x = start.x + trackWidth * frac;
            drawList->AddLine({x, start.y - radius}, {x, start.y + radius}, trackColor);
        }

        auto marker = ImVec2{start.x + trackWidth * std::clamp(position, 0.f, 1.f), start.y};
        drawList->AddCircleFilled(marker, radius, ImGui::GetColorU32(ImGuiCol_SliderGrabActive));

        if(hasLabels) {
            auto y = origin.y + trackHeight + gap;
            auto textColor = ImGui::GetColorU32(ImGuiCol_Text);
            auto rightSize = ImGui::CalcTextSize(rightLabel);

            drawList->AddText({origin.x, y}, textColor, leftLabel);
            drawList->AddText({origin.x + width - rightSize.x, y}, textColor, rightLabel);
        }
    }
}