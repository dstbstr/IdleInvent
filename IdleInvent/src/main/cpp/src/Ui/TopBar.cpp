#include "Ui/TopBar.h"
#include "Ui/Ui.h"
#include "Ui/Screens.h"
#include "Settings/GameSettings.h"

#include "InventLib/GameState/GameState.h"

#include "Core/DesignPatterns/ServiceLocator.h"
#include "Core/Instrumentation/Logging.h"

#include "imgui.h"

namespace {
    Invent::GameState* gameState {nullptr};
    GameSettings* gameSettings {nullptr};
}

namespace Ui::TopBar {
    bool Initialize() {
        gameState = &ServiceLocator::Get().GetRequired<Invent::GameState>();
        gameSettings = &ServiceLocator::Get().GetOrCreate<GameSettings>();

        return true;
    }

    void Render() {
#ifdef DEBUG
        DR_ASSERT_MSG(gameState, "Initialize must be called before Render");
#endif

        const auto& frameRate = ImGui::GetIO().Framerate;
        ImGui::Begin("TopBar", nullptr, BaseFlags);
        if(gameSettings->showFps){
            ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2);
            ImGui::Text("%.1f FPS", frameRate);
        } else {
            ImGui::Text("");
        }

        ImGui::BeginTable("ResourceTable", gameState->CurrentResources.size());
        for(const auto& [name, resource] : gameState->CurrentResources) {
            ImGui::TableNextColumn();
            ImGui::Text("%s", std::format("{}: {}", ToString(name), resource.Current).c_str());
        }
        ImGui::EndTable();

        if(ImGui::Button("Settings")) {
            Screens::SetActiveScreen(Screen::Settings);
        }

        ImGui::End();
    }
}