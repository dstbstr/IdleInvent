#include "GhostHunter/Ui/Screens/InvestigateScreen.h"
#include "GhostHunter/Ui/Ui.h"
#include "GhostHunter/Locations/Locations.h"
#include "GhostHunter/Formatting.h"
#include "GhostHunter/GameState/Life.h"

#include "Utilities/EnumUtils.h"
#include "Mechanics/Purchasable.h"

#include <imgui.h>

namespace {
    GhostHunter::LocationName currentLocation = GhostHunter::LocationName::Unset;
    GhostHunter::Life* life{nullptr};

    void RenderRentLocation() {
        using namespace GhostHunter;
        auto locations = Enum::GetAllValues<LocationName>();
        auto purchaseables = Purchasables::GetAvailable<LocationName>();
        for(const auto& [location, cost]: purchaseables) {
            ImGui::PushID(static_cast<int>(location));
            auto text = std::format("{} ({})", ToString(location), cost[ResourceName::Cash].Current);
            if(ImGui::Button(text.c_str())) {
                Purchasables::TryPurchase(location, life->GetInventory().Resources, BuyOnce::No);
                currentLocation = location;
            }
            ImGui::PopID();
        }
    }

    void RenderUseLocation() {
        ImGui::Text("Investigating %s", GhostHunter::ToString(currentLocation).c_str());
        // begin investigation
    }
}

namespace GhostHunter::Ui::Screens::Investigate {
    bool Initialize() { 
        life = &ServiceLocator::Get().GetRequired<Life>();
        return true; 
    }

    void ShutDown() {}

    void Render() {
        if(currentLocation != LocationName::Unset) {
            RenderUseLocation();
        } else {
            RenderRentLocation();
        }
    }
} // namespace GhostHunter::Ui::Screens::Investigate