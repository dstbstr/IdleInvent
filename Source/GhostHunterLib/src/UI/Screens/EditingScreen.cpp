#include "GhostHunter/Ui/Screens/EditingScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Editing {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() { ImGui::Text("Editing"); }
} // namespace GhostHunter::Ui::Screens::Editing