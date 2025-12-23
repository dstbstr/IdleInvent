#include "GhostHunter/Ui/Screens/MediaScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Media {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Begin("Media", nullptr, BaseFlags);
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::Media