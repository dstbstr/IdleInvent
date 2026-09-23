#include "Walker/Ui/VehicleSelector.h"

#include <algorithm>

namespace Walker::WalkerUi {
	bool VehicleSelector(const char* id, std::span<const VehicleKind> choices, VehicleKind& selected) {
        auto current = std::ranges::find(choices, selected);
        if (current == choices.end()) return false;

        auto name = ToString(selected);
        auto result = selected;
        auto& style = ImGui::GetStyle();
        auto rowWidth =
            ImGui::CalcTextSize("<").x +
            ImGui::CalcTextSize(">").x +
            style.FramePadding.x * 4.f +
            ImGui::CalcTextSize(name.c_str()).x +
            style.ItemSpacing.x * 2.f;

        auto startX = ImGui::GetCursorPosX();
        auto offset = std::max(0.f, (ImGui::GetContentRegionAvail().x - rowWidth) * 0.5f);
        ImGui::SetCursorPosX(startX + offset);

        ImGui::PushID(id);
        auto index = static_cast<size_t>(current - choices.begin());
        ImGui::BeginDisabled(index == 0);
        if (ImGui::Button("<")) result = choices[index - 1];
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(name.c_str());
        ImGui::SameLine();

        ImGui::BeginDisabled(index == choices.size() - 1);
        if (ImGui::Button(">")) result = choices[index + 1];
        ImGui::EndDisabled();

        ImGui::SetCursorPosX(startX);
        ImGui::PopID();

		auto changed = result != selected;
        selected = result;
        return changed;
	}

    bool EndpointSelector(const char* id, std::span<const std::unique_ptr<EndpointInstance>> choices, EndpointInstance*& selected) {
        auto current = std::ranges::find(choices, selected, [](const auto& endpoint) { return endpoint.get(); });
        if (!selected || current == choices.end()) return false;
        
        auto result = selected;
        auto& style = ImGui::GetStyle();
        auto rowWidth =
            ImGui::CalcTextSize("<").x +
            ImGui::CalcTextSize(">").x +
            style.FramePadding.x * 4.f +
            ImGui::CalcTextSize(selected->Name.c_str()).x +
            style.ItemSpacing.x * 2.f;

        auto startX = ImGui::GetCursorPosX();
        auto offset = std::max(0.f, (ImGui::GetContentRegionAvail().x - rowWidth) * 0.5f);
        ImGui::SetCursorPosX(startX + offset);

        ImGui::PushID(id);
        auto index = static_cast<size_t>(current - choices.begin());
        ImGui::BeginDisabled(index == 0);
        if (ImGui::Button("<")) result = choices[index - 1].get();
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(selected->Name.c_str());
        ImGui::SameLine();

        ImGui::BeginDisabled(index == choices.size() - 1);
        if (ImGui::Button(">")) result = choices[index + 1].get();
        ImGui::EndDisabled();

        ImGui::SetCursorPosX(startX);
        ImGui::PopID();

        auto changed = result != selected;
        selected = result;
        return changed;
    }
}