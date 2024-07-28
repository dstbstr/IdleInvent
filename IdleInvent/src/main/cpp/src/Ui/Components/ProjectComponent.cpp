#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Life.h"
#include "InventLib/Projects/Project.h"

#include "Core/Constexpr/ConstexprStrUtils.h"

#include <imgui.h>

namespace Ui::Components::Project {
    void Render(Invent::Life& life, Invent::Project& project, StartCondition startCondition) {
        ImGui::PushID(&project);

        auto disableWorkers = startCondition == StartCondition::RequireResources && project.ResourceProgress.AreAnyLessThan(project.ResourceCost);
        if(disableWorkers) {
            ImGui::BeginDisabled();
        }
        auto disableMinus = project.CurrentWorkers == 0;
        if(disableMinus) {
            ImGui::BeginDisabled();
        }
        if(ImGui::SmallButton("-")) {
            project.CurrentWorkers--;
            life.AvailableWorkers++;
        }
        if(disableMinus) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        ImGui::Text("%s", std::format("{}", project.CurrentWorkers).c_str());
        ImGui::SameLine();
        auto disablePlus = life.AvailableWorkers == 0;
        if(disablePlus) {
            ImGui::BeginDisabled();
        }
        if(ImGui::SmallButton("+")) {
            project.CurrentWorkers++;
            life.AvailableWorkers--;
        }
        if(disablePlus) {
            ImGui::EndDisabled();
        }
        if(disableWorkers) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        ImGui::Text("%s", project.Name.c_str());

        ImGui::SameLine();
        if(ImGui::Button("Invest")) { project.Invest(life.Resources); }

        ImGui::SameLine();
        auto timeProgress = static_cast<float>(project.TimeProgress.count()) / static_cast<float>(project.TimeCost.count());
        auto remainingTime = (project.TimeCost.count() - project.TimeProgress.count()) / std::max(size_t(1), project.CurrentWorkers);
        auto label = std::format("{}", Constexpr::TimeString(remainingTime));
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