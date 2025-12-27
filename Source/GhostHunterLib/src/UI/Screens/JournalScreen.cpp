#include "GhostHunter/Ui/Screens/JournalScreen.h"
#include "GhostHunter/Ui/Ui.h"

#include <imgui.h>

namespace GhostHunter::Ui::Screens::Journal {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        ImGui::Text("Journal");
    }
} // namespace GhostHunter::Ui::Screens::Journal