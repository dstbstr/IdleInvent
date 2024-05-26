#include "Ui/Screens/Storages.h"

#include "Ui/Ui.h"
#include "imgui.h"

namespace Ui::Screens::Storages {
    bool Initialize() {
        return true;
    }
    void Render() {
        ImGui::Begin("StoragesScreen", nullptr, BaseFlags);
        ImGui::Text("Storages");
        ImGui::End();
    }
}