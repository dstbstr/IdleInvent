#include "Ui/Screens/Home.h"

#include "Ui/Ui.h"
#include "Ui/Components/ProjectComponent.h"

#include "InventLib/Character/Life.h"
#include "InventLib/Character/Society.h"
#include "InventLib/GameState/GameState.h"
#include "InventLib/GameState/GameTime.h"
#include "InventLib/Projects/Project.h"
#include "InventLib/Resources/ResourceConverters.h"

#include "Core/Constexpr/ConstexprStrUtils.h"
#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <format>
#include <algorithm>

namespace {
    Invent::Life* life{nullptr};
    int purchaseChoice = 0;
    ImVec2 buttonSize = ImVec2(0, 0);

    enum struct PurchaseAmount { One, Ten, Half, Max};

    void RenderPurchaseAmounts() {
        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x * 0.7f);
        ImGui::BeginTable("PurchaseAmountTable", 4);
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1", purchaseChoice == 0)) {
            purchaseChoice = static_cast<int>(PurchaseAmount::One);
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("10", purchaseChoice == 1)) {
            purchaseChoice = static_cast<int>(PurchaseAmount::Ten);
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("1/2", purchaseChoice == 2)) {
            purchaseChoice = static_cast<int>(PurchaseAmount::Half);
        }
        ImGui::TableNextColumn();
        if(ImGui::Selectable("Max", purchaseChoice == 3)) {
            purchaseChoice = static_cast<int>(PurchaseAmount::Max);
        }
        ImGui::EndTable();
    }

    void RenderActions() {
        if(ImGui::Button("Work", buttonSize)) {
            life->Work();
        }
    }

    void RenderConverters(PurchaseAmount purchaseAmount) {
        auto getCount = [purchaseAmount](const Invent::ResourceConversion& conversion) {
            auto count = conversion.GetMaxCount(life->Resources);
            switch(purchaseAmount) {
                case PurchaseAmount::One: return std::min(size_t(1), count);
                case PurchaseAmount::Ten: return count >= 10 ? size_t(10) : 0;
                case PurchaseAmount::Half: return count > 1 ? count / 2 : 0;
                case PurchaseAmount::Max: return count;
            }
        };
        for (const auto &converter: life->ResourceConverters) {
            auto count = getCount(converter);
            if (count == 0) {
                ImGui::BeginDisabled();
                ImGui::Button(converter.Name.c_str(), buttonSize);
                ImGui::EndDisabled();
            } else {
                if (ImGui::Button(std::format("{} ({})", converter.Name, count).c_str(), buttonSize)) {
                    converter.Convert(life->Resources, count);
                }
            }
        }
    }

    void RenderPopulationActions() {
        for(auto& project : life->Projects.at(Invent::ProjectType::Population)) {
            Ui::Components::Project::Render(*life, project);
        }
    }
}

namespace Ui::Screens::Home {
    bool Initialize() {
        life = &ServiceLocator::Get().GetRequired<Invent::Society>().CurrentLife;
        return true;
    }

    void ShutDown() {}
    
    void Render() { 
        buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 50);

        ImGui::Begin("Home", nullptr, BaseFlags);
        RenderPurchaseAmounts();
        if(ImGui::CollapsingHeader("Actions", ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderActions();
        }
        if(ImGui::CollapsingHeader("Converters", ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderConverters(static_cast<PurchaseAmount>(purchaseChoice));
        }
        if(ImGui::CollapsingHeader("Population", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("%s", std::format("Population: {}/{}", life->CurrentPopulation, life->MaxPopulation).c_str());
            RenderPopulationActions();
        }

        ImGui::End();
    }
}