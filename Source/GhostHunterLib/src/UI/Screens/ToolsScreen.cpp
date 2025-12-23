#include "GhostHunter/Ui/Screens/ToolsScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Tools {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("Tools", nullptr, BaseFlags);
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::Tools