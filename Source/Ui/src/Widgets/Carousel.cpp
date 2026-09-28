#include "Ui/Widgets/Carousel.h"

#include <imgui.h>

#include <algorithm>

namespace Ui {
	bool Carousel(const char* id, const char* label, size_t count, size_t& selectedIndex) {
        if(count == 0 || selectedIndex >= count) return false;

        auto result = selectedIndex;
        auto& style = ImGui::GetStyle();
        auto rowWidth =
            ImGui::CalcTextSize("<").x +
            ImGui::CalcTextSize(">").x +
            style.FramePadding.x * 4.f +
            ImGui::CalcTextSize(label).x +
            style.ItemSpacing.x * 2.f;

        auto startX = ImGui::GetCursorPosX();
        auto offset = std::max(0.f, (ImGui::GetContentRegionAvail().x - rowWidth) * 0.5f);
        ImGui::SetCursorPosX(startX + offset);

        ImGui::PushID(id);
        ImGui::BeginDisabled(selectedIndex == 0);
        if (ImGui::Button("<")) result--;
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();

        ImGui::BeginDisabled(selectedIndex == count - 1);
        if (ImGui::Button(">")) result++;
        ImGui::EndDisabled();

        ImGui::SetCursorPosX(startX);
        ImGui::PopID();

        auto changed = result != selectedIndex;
        selectedIndex = result;
        return changed;
	}
}