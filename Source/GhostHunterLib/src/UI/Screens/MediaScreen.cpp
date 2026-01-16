#include "GhostHunter/Ui/Screens/MediaScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Inventory/Inventory.h"
#include "GhostHunter/Formatting.h"

#include "DesignPatterns/ServiceLocator.h"

#include <imgui.h>

namespace {
}

namespace GhostHunter::Ui::Screens::Media {
    bool Initialize() { 
        return true; 
    }

    void ShutDown() {}

    void Render() {
        const auto& allMedia = GhostHunter::Inventory::GetMedia();
        ImGui::Text("Inventory Size: %zu", allMedia.size());

        for(size_t i = 0; i < allMedia.size(); i++) {
            const auto& media = allMedia[i];
            ImGui::Text("%s", std::format("{}: {}", i, media).c_str());
            if(ImGui::Button("Sell")) {
                GhostHunter::Inventory::SellMedia(i);
            }
        }

        if(ImGui::Button("Sell All")) {
            GhostHunter::Inventory::SellAllMedia();
        }
    }
} // namespace GhostHunter::Ui::Screens::Media