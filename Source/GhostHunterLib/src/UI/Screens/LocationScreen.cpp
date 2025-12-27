#include "GhostHunter/Ui/Screens/LocationScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Location {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Location");
    }
} // namespace GhostHunter::Ui::Screens::Location