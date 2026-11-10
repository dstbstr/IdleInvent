#pragma once

#include <imgui.h>

#include "Iduel/Ui/Screens/IduelScreens.h"

namespace Iduel::IduelUi::Screens::Start {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() {
        ImGui::Text("Start Screen");
        if(ImGui::Button("Start")) {
            GetScreenManager().SetActiveScreen(Screen::Match);
        }
    }
}