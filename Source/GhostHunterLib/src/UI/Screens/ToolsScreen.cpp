#include "GhostHunter/Ui/Screens/ToolsScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Tools {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Tools");
    }
} // namespace GhostHunter::Ui::Screens::Tools