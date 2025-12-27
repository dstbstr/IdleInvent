#include "GhostHunter/Ui/Screens/PrestigeScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Prestige {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Prestige");
    }
} // namespace GhostHunter::Ui::Screens::Prestige