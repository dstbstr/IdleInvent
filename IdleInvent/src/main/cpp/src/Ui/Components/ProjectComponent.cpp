#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Life.h"
#include "InventLib/Projects/Project.h"

#include "Core/Constexpr/ConstexprStrUtils.h"

#include <imgui.h>

namespace Ui::Components::Project {
    void RenderPlusMinus(size_t& available, size_t& spent, Invent::PurchaseAmount purchaseAmount) {
        auto disableMinus = spent == 0;
        if(disableMinus) {
            ImGui::BeginDisabled();
        }
        if(ImGui::SmallButton("-")) {
            auto count = Invent::GetPurchaseCount(spent, purchaseAmount);
            if(count == 0 && spent > 0) {
                count = spent;
            }
            spent -= count;
            available += count;
        }
        if(disableMinus) {
            ImGui::EndDisabled();
        }

        ImGui::SameLine();
        ImGui::Text("%s", std::format("{}", spent).c_str());
        ImGui::SameLine();
        auto disablePlus = available == 0;
        if(disablePlus) {
            ImGui::BeginDisabled();
        }
        if(ImGui::SmallButton("+")) {
            auto count = Invent::GetPurchaseCount(available, purchaseAmount);
            spent += count;
            available -= count;
        }
        if(disablePlus) {
            ImGui::EndDisabled();
        }
    }

    void Render(Invent::Life& life, Invent::Project& project, Invent::PurchaseAmount purchaseAmount) {
        ImGui::PushID(&project);

        RenderPlusMinus(life.AvailableWorkers, project.CurrentWorkers, purchaseAmount);    

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