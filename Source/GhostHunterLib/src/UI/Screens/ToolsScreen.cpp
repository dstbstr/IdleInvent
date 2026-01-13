#include "GhostHunter/Ui/Screens/ToolsScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"
#include "Utilities/EnumUtils.h"

#include <imgui.h>
#include <ranges>
#include <algorithm>

namespace GhostHunter::Ui::Screens::Tools {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        auto& tools = Inventory::GetTools();
        auto toolNames = Enum::GetAllValues<ToolName>();
        auto ownedToolNames =
            tools 
            | std::views::transform([](const Tool& tool) { return tool.Name; }) 
            | std::ranges::to<std::vector>();

        int id = 0;
        for(const auto& toolName: toolNames) {
            bool isOwned = (std::find(ownedToolNames.begin(), ownedToolNames.end(), toolName) != ownedToolNames.end());
            if(isOwned) continue;
            auto tool = ToString(toolName);
            ImGui::Text("%s", tool.c_str());
            ImGui::SameLine();
            ImGui::PushID(id++);
            if(ImGui::Button("Purchase")) {
                tools.push_back(Tool(toolName, QualityType::Awful));
                std::sort(tools.begin(), tools.end(), [](const Tool& lhs, const Tool& rhs) {
                    return ToString(lhs.Name) < ToString(rhs.Name);
                });
            }
            ImGui::PopID();
        }

        if(tools.size() != toolNames.size() && tools.size() > 0) {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        for(auto& tool : tools) {
            ImGui::Text("%s (%s)", ToString(tool.Name).c_str(), ToString(tool.Quality).c_str());
            ImGui::SameLine();
            ImGui::PushID(id++);
            if(ImGui::Button("Upgrade")) {
                tool.Upgrade();
            }
            ImGui::PopID();
        }
    }
} // namespace GhostHunter::Ui::Screens::Tools