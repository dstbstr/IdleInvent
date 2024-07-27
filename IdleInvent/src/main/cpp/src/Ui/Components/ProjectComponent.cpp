#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Life.h"
#include "InventLib/Projects/Project.h"

#include "Core/Constexpr/ConstexprStrUtils.h"

#include <imgui.h>

namespace Ui::Components::Project {
    void Render(Invent::Life& life, Invent::Project& project) {
        ImGui::PushID(&project);
        if(project.Active || life.AvailableWorkers > 0) {
            if(ImGui::Checkbox(project.Name.c_str(), &project.Active)) { life.AvailableWorkers += project.Active ? -1 : 1; }
        } else {
            ImGui::Text("%s", project.Name.c_str());
        }
        ImGui::SameLine();
        if(ImGui::Button("Invest")) { project.Invest(life.Resources); }
        ImGui::SameLine();

        auto timeProgress = static_cast<float>(project.TimeProgress.count()) / static_cast<float>(project.TimeCost.count());
        auto label = std::format("{}", Constexpr::TimeString(project.TimeCost.count() - project.TimeProgress.count()));
        ImGui::ProgressBar(timeProgress, ImVec2(-1, 0), label.c_str());

        if(ImGui::CollapsingHeader("ResourceCosts", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanTextWidth)) {
            ImGui::TextWrapped("%s", project.Description.c_str());
            ImGui::TextWrapped("%s", project.EffectDescription.c_str());
            for(const auto& resource: Invent::AllResources()) {
                auto remaining = project.ResourceCost.at(resource).Current - project.ResourceProgress.at(resource).Current;
                if(remaining > 0) {
                    auto name = Invent::ToString(resource);
                    ImGui::Text("%s", std::format("{} {}", name, remaining).c_str());
                }
            }
        }
        ImGui::PopID();
    }
} // namespace Ui::Components::Project