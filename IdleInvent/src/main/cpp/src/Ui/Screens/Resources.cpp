#include "Ui/Screens/Resources.h"

#include "Ui/Ui.h"
#include "imgui.h"

namespace Ui::Screens::Resources {
    bool Initialize() {
        return true;
    }
    void Render() { 
        ImGui::Begin("ResourceScreen", nullptr, BaseFlags);
        ImGui::Text("ResourceScreen");
        ImGui::End();
    }
}