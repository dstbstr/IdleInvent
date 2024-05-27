#include "Ui/Screens/Resources.h"

#include "Ui/Ui.h"

#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/ServiceLocator.h"

#include "imgui.h"

#include <format>

namespace Ui::Screens::Resources {
    bool Initialize() {
        return true;
    }
    void Render() { 
        static auto& gameState = ServiceLocator::Get().GetRequired<Invent::GameState>();

        ImGui::Begin("ResourceScreen", nullptr, BaseFlags);
        ImGui::Text("%s", std::format("Labor: {}", gameState.CurrentResources[Invent::ResourceName::Labor]).c_str());
        ImGui::Text("%s", std::format("Knowledge: {}", gameState.CurrentResources[Invent::ResourceName::Knowledge]).c_str());
        ImGui::Text("%s", std::format("Wealth: {}", gameState.CurrentResources[Invent::ResourceName::Wealth]).c_str());
        ImGui::Text("%s", std::format("Influence: {}", gameState.CurrentResources[Invent::ResourceName::Influence]).c_str());
        ImGui::Text("%s", std::format("Magic: {}", gameState.CurrentResources[Invent::ResourceName::Magic]).c_str());
        ImGui::End();
    }
}