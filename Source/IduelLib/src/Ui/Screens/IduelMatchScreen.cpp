#pragma once

#include "Iduel/Ui/Screens/IduelScreens.h"

#include <imgui.h>

namespace Iduel::IduelUi::Screens::Match {
    bool Initialize() { return true;}
    void ShutDown() {}

    void Render() {
        ImGui::Text("Match Screen");
        if(ImGui::Button("End")) {
			GetScreenManager().SetActiveScreen(Screen::Review);
        }
    }
}