#include "TechMaze/Ui/TopContent.h"
#include "TechMaze/Ui/Ui.h"
#include "Ui/UiUtil.h"

#include <imgui.h>
#include <format>

namespace TechMaze::Ui::Screens::TopContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        ImGui::Begin("Top Content", nullptr, BaseFlags);
        const auto& frameRate = ImGui::GetIO().Framerate;
        TextCenteredX(std::format("{:.1f} FPS", frameRate).c_str());

        ImGui::End();
    }
} // namespace TechMaze::Ui::Screens::TopContent