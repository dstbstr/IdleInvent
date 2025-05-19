#include "TechMaze/Ui/BottomContent.h"
#include "TechMaze/Ui/Ui.h"
#include <Ui/UiUtil.h>

#include <imgui.h>

namespace TechMaze::Ui::Screens::BottomContent {
    void Initialize() {
        // Initialization code here
    }
    void Render() {
        //show current hint
        //show current target
        //randomize current target
        ImGui::Begin("Bottom Content", nullptr, BaseFlags);
        TextCenteredX("Bottom Content");
        ImGui::End();
    }
} // namespace TechMaze::Ui::Screens::BottomContent