#include "GhostHunter/Ui/Screens/LocationScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Formatting.h"

#include <imgui.h>

namespace {
    GhostHunter::LocationName currentLocation = GhostHunter::LocationName::Unset;
}

namespace GhostHunter::Ui::Screens::Location {
    bool Initialize() { return true; }

    void ShutDown() {}

    void Render() {
        auto locations = GetAllLocationNames();
        for (const auto& location : locations) {
            bool isSelected = (currentLocation == location);
            if (ImGui::Selectable(ToString(location).c_str(), isSelected)) {
                currentLocation = location;
            }
        }

        if(currentLocation != LocationName::Unset) {
            ImGui::Text("Selected Location: %s", ToString(currentLocation).c_str());
        } else {
            ImGui::Text("No Location Selected");
        }
    }
} // namespace GhostHunter::Ui::Screens::Location