#include "GhostHunter/Ui/Screens/SettingsScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Settings {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("Settings", nullptr, BaseFlags);
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::Settings