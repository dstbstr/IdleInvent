#include "GhostHunter/Ui/Screens/ToolsScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"
#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"
#include "DesignPatterns/ServiceLocator.h"

#include <imgui.h>
#include <ranges>
#include <algorithm>

namespace {
    ResourceCollection* resources{nullptr};
}

namespace GhostHunter::Ui::Screens::Tools {
    bool Initialize() { 
        resources = &ServiceLocator::Get().GetRequired<ResourceCollection>();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        auto& owned = Inventory::GetTools();
        auto available = Purchasables::GetAvailable<ToolName>();
        
        for(const auto&[id, cost] : available) {
            auto name = ToString(id);
            ImGui::Text("%s", name.c_str());
            ImGui::SameLine();
            ImGui::PushID(static_cast<int>(id));
            auto disabled = cost > *resources;
            if(disabled) ImGui::BeginDisabled();
            if(ImGui::Button("Purchase")) {
                Purchasables::TryPurchase<ToolName>(id, *resources, BuyOnce::Yes);
            }
            if(disabled) ImGui::EndDisabled();
            ImGui::PopID();
        }
        //auto toolNames = Enum::GetAllValues<ToolName>();
        //auto ownedToolNames =
        //    tools 
        //    | std::views::transform([](const Tool& tool) { return tool.Name; }) 
        //    | std::ranges::to<std::vector>();

        //int id = 0;
        //for(const auto& toolName: toolNames) {
        //    bool isOwned = (std::find(ownedToolNames.begin(), ownedToolNames.end(), toolName) != ownedToolNames.end());
        //    if(isOwned) continue;
        //    auto tool = ToString(toolName);
        //    ImGui::Text("%s", tool.c_str());
        //    ImGui::SameLine();
        //    ImGui::PushID(id++);
        //    if(ImGui::Button("Purchase")) {
        //        tools.push_back(Tool(toolName, QualityType::Awful));
        //        std::sort(tools.begin(), tools.end(), [](const Tool& lhs, const Tool& rhs) {
        //            return ToString(lhs.Name) < ToString(rhs.Name);
        //        });
        //    }
        //    ImGui::PopID();
        //}

        if(available.size() > 0 && owned.size() > 0) {
        //if(tools.size() != toolNames.size() && tools.size() > 0) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        int id = 0;
        for(auto& tool : owned) {
            ImGui::Text("%s", tool.Describe().c_str());
            ImGui::SameLine();
            ImGui::PushID(id++);
            if(ImGui::Button("Upgrade")) {
                tool.Upgrade();
            }
            ImGui::PopID();
        }
    }
} // namespace GhostHunter::Ui::Screens::Tools