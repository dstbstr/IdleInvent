#include "Ui/TopBar.h"
#include "Ui/Ui.h"

#include "imgui.h"

namespace Ui::TopBar {
    bool Initialize() {
        return true;
    }
    void Render() {
        const auto& frameRate = ImGui::GetIO().Framerate;
        ImGui::Begin("TopBar", nullptr, BaseFlags | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SameLine(ImGui::GetContentRegionAvail().x / 2);
        ImGui::Text("%.1f FPS", frameRate);
        ImGui::End();
    }
}