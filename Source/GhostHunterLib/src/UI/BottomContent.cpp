#include "GhostHunter/Ui/BottomContent.h"
#include "GhostHunter/Ui/Ui.h"
#include <Ui/UiUtil.h>

#include <imgui.h>

namespace GhostHunter::Ui::Screens::BottomContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        ImGui::Begin("Bottom Content", nullptr, BaseFlags);
        TextCenteredX("Bottom Content");
        ImGui::End();
    }
} // namespace GhostHunter::Ui::Screens::BottomContent