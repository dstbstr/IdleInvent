#pragma once

#include "Iduel/Ui/Screens/IduelScreens.h"
#include <imgui.h>


namespace Iduel::IduelUi::Screens::Start {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() {
        ImGui::Text("Start Screen");
        if(ImGui::Button("Start")) {
            GetScreenManager().SetActiveScreen(Screen::Match);
        } else if(ImGui::Button("Editor")) {
            GetScreenManager().SetActiveScreen(Screen::Editor);
        }
    }
}