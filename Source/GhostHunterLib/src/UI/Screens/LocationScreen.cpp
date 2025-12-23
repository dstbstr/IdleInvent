#include "GhostHunter/Ui/Screens/LocationScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Location {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("Location", nullptr, BaseFlags);
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::Location