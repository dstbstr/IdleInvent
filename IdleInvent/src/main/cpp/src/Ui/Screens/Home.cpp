#include "Ui/Screens/Home.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"
#include "InventLib/Character/Life.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/Projects/Project.h"
#include "InventLib/Resources/ResourceConverters.h"
#include "InventLib/Settings/GameSettings.h"
#include "Ui/Components/ProjectComponent.h"
#include "Ui/Ui.h"
#include "imgui.h"

#include <algorithm>
#include <format>

namespace {
    Invent::Society* society{nullptr};
    Invent::GameSettings* gameSettings{nullptr};

    size_t lastConvertAllStart = 0;
    int purchaseChoice = 0;
    ImVec2 buttonSize = ImVec2(0, 0);

    enum struct PurchaseAmount { One, Ten, Half, Max };

    void RenderPurchaseAmounts() {
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.7f);
        ImGui::BeginTable("PurchaseAmountTable", 4);
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1", purchaseChoice == 0)) { purchaseChoice = static_cast<int>(PurchaseAmount::One); }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("10", purchaseChoice == 1)) { purchaseChoice = static_cast<int>(PurchaseAmount::Ten); }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1/2", purchaseChoice == 2)) { purchaseChoice = static_cast<int>(PurchaseAmount::Half); }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("Max", purchaseChoice == 3)) { purchaseChoice = static_cast<int>(PurchaseAmount::Max); }
        ImGui::EndTable();
    }

    size_t GetPurchaseCount(const Invent::ResourceConversion& conversion) {
        auto purchaseAmount = static_cast<PurchaseAmount>(purchaseChoice);
        auto count = conversion.GetMaxCount(society->CurrentLife.Resources);
        switch(purchaseAmount) {
        case PurchaseAmount::One: return std::min(size_t(1), count);
        case PurchaseAmount::Ten: return count >= 10 ? size_t(10) : 0;
        case PurchaseAmount::Half: return count > 1 ? count / 2 : 0;
        case PurchaseAmount::Max: return count;
        }
    }

    void RenderActions() {
        if(ImGui::Button("Work", buttonSize)) { society->CurrentLife.Work(); }

        if(society->HasConvertAll) {
            if(ImGui::Button("Convert All", buttonSize)) {
                // each time this button is clicked, start with the next converter
                auto& converters = society->CurrentLife.ResourceConverters;
                for(auto i = 0u; i < converters.size(); i++) {
                    auto index = (i + lastConvertAllStart) % converters.size();
                    auto& converter = converters.at(index);
                    converter.Convert(society->CurrentLife.Resources, GetPurchaseCount(converter));
                }
                lastConvertAllStart++;
            }
        }

        if(society->HasInvestAll) {
            if(ImGui::Button("Invest All", buttonSize)) {
                // Invest in active projects before inactive
                for(auto priority: gameSettings->ProjectPriority) {
                    for(auto& project: society->CurrentLife.Projects.at(priority)) {
                        if(project.CurrentWorkers > 0 || project.TimeCost == project.TimeProgress) { project.Invest(society->CurrentLife.Resources); }
                    }
                }
                for(auto priority: gameSettings->ProjectPriority) {
                    for(auto& project: society->CurrentLife.Projects.at(priority)) {
                        if(project.CurrentWorkers == 0 && project.TimeProgress < project.TimeCost) { project.Invest(society->CurrentLife.Resources); }
                    }
                }
            }
        }
        if(society->HasWorkerShift) {
            if(ImGui::Button("Assign Workers", buttonSize)) { society->CurrentLife.ShiftWorkers(); }
        }
        if(ImGui::Button("Unassign Workers", buttonSize)) { society->CurrentLife.ClearWorkers(); }
    }

    void RenderConverters() {
        for(const auto& converter: society->CurrentLife.ResourceConverters) {
            auto count = GetPurchaseCount(converter);
            if(count == 0) {
                ImGui::BeginDisabled();
                ImGui::Button(converter.Name.c_str(), buttonSize);
                ImGui::EndDisabled();
            } else {
                if(ImGui::Button(std::format("{} ({})", converter.Name, count).c_str(), buttonSize)) { converter.Convert(society->CurrentLife.Resources, count); }
            }
        }
    }

    void RenderPopulationActions() {
        for(auto& project: society->CurrentLife.Projects.at(Invent::ProjectType::Population)) {
            Ui::Components::Project::Render(society->CurrentLife, project);
        }
    }
} // namespace

namespace Ui::Screens::Home {
    bool Initialize() {
        society = &ServiceLocator::Get().GetRequired<Invent::Society>();
        gameSettings = &ServiceLocator::Get().GetRequired<Invent::GameSettings>();
        return true;
    }

    void ShutDown() {}

    void Render() {
        buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 50);

        ImGui::Begin("Home", nullptr, BaseFlags);
        RenderPurchaseAmounts();
        if(ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen)) { RenderActions(); }
        if(ImGui::CollapsingHeader("Converters", ImGuiTreeNodeFlags_DefaultOpen)) { RenderConverters(); }
        if(ImGui::CollapsingHeader("Population", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("%s", std::format("Population: {}/{}", society->CurrentLife.CurrentPopulation, society->CurrentLife.MaxPopulation).c_str());
            RenderPopulationActions();
        }

        ImGui::End();
    }
} // namespace Ui::Screens::Home