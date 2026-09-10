#include "GhostHunter/UI/Screens/PrestigeScreen.h"
#include "GhostHunter/UI/Ui.h"

namespace GhostHunter::Ui::Screens::Prestige {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Prestige");
    }
} // namespace GhostHunter::Ui::Screens::Prestige