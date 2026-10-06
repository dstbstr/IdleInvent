#include "Walker/Ui/Screens/MilestonesScreen.h"

#include <imgui.h>

namespace Walker::WalkerUi::Screens::Milestones {
    bool Initialize() {
        return true;
    }
    void ShutDown() {}

    void Render() {
        ImGui::TextUnformatted("Milestones");
    }
}
