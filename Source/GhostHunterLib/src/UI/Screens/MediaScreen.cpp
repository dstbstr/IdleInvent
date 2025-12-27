#include "GhostHunter/Ui/Screens/MediaScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Media {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Media");
    }
} // namespace GhostHunter::Ui::Screens::Media