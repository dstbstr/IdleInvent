#include "Platform/Graphics.h"
#include "GhostHunter/Ui/MainContent.h"
#include "GhostHunter/Ui/Ui.h"

#include <Ui/ChildBuilder.h>
#include <Ui/UiUtil.h>
#include <Platform/NumTypes.h>

#include <imgui.h>
#include <array>

namespace GhostHunter::Ui::Screens::MainContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        ImGui::Begin("Main Content", nullptr, BaseFlags);
        ImGui::Text("Main Content Area");
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::MainContent