#include "Ui/Screens/Purchases.h"

#include "Ui/Ui.h"
#include "imgui.h"

namespace Ui::Screens::Purchases {
    bool Initialize() {
        return true;
    }
    void Render() {
        ImGui::Begin("PurchasesScreen", nullptr, BaseFlags);
        ImGui::Text("Purchases");
        ImGui::End();
    }
}