#include "Ui/Screens/Achievements.h"

#include "Ui/Ui.h"
#include "imgui.h"

namespace Ui::Screens::Achievements {
    bool Initialize() {
        return true;
    }
    void Render() {
        ImGui::Begin("AchievementsScreen", nullptr, BaseFlags);
        ImGui::Text("Achievements");
        ImGui::End();
    }
}