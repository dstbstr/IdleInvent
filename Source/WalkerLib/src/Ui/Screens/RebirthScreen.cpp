#include "Walker/Ui/Screens/RebirthScreen.h"

#include <imgui.h>

namespace Walker::WalkerUi::Screens::Rebirth {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() { ImGui::TextUnformatted("Rebirth"); }
} // namespace Walker::WalkerUi::Screens::Crew
