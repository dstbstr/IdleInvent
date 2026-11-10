#pragma once

#include "Iduel/Ui/Screens/IduelScreens.h"

#include <imgui.h>

namespace Iduel::IduelUi::Screens::Review {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() {
        ImGui::Text("Review Screen");
		if (ImGui::Button("Restart")) {
			GetScreenManager().SetActiveScreen(Screen::Start);
		}
    }
}