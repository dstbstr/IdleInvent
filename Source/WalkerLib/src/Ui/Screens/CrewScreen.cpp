#include "Walker/Ui/Screens/CrewScreen.h"

#include <imgui.h>

namespace Walker::WalkerUi::Screens::Crew {
    bool Initialize() { return true; }
    void ShutDown() {}

    void Render() { ImGui::TextUnformatted("Crew"); }
} // namespace Walker::WalkerUi::Screens::Crew
