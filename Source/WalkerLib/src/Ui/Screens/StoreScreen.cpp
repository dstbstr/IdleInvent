#include "Walker/Ui/Screens/StoreScreen.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Ui/VehicleSelector.h"

#include <Utilities/EnumUtils.h>

#include <imgui.h>
#include <algorithm>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    VehicleKind SelectedKind{VehicleKind::Foot};

    void RenderStats(const VehicleDetails& details) {
        ImGui::Text("Cost: %s", details.Cost.ToScientific().c_str());
		ImGui::Text("Max Speed: %s", details.MaxSpeed.ToScientific().c_str());
        ImGui::Text("Capacity: %s", details.MaxCapacity.ToScientific().c_str());
		ImGui::Text("Base Acceleration: %s", details.BaseAcceleration.ToScientific().c_str());
		ImGui::Text("Powered Acceleration: %s", details.PoweredAcceleration.ToScientific().c_str());
		ImGui::Text("Fuel Efficiency: %s", details.FuelEfficiency.ToScientific().c_str());
    }

    void RenderSelector() {
        WalkerUi::VehicleSelector("VehicleSelector", Enum::GetAllValues<VehicleKind>(), SelectedKind);

		const auto& details = GetVehicleDetails(SelectedKind);
		RenderStats(details);

        ImGui::BeginDisabled(Home->Vehicles.Contains(SelectedKind) || !Home->Funds.CanAfford(details.Cost));
        if(ImGui::Button("Purchase")) {
            if(Home->Funds.Spend(details.Cost)) {
                if(!Home->Vehicles.Add(SelectedKind)) {
                    // refund
					Home->Funds.Add(details.Cost);
                }
            }
        }
        ImGui::EndDisabled();
    }
}

namespace Walker::WalkerUi::Screens::Store {
    bool Initialize() { 
		Home = &ServiceLocator::Get().GetRequired<HomeBase>();
        return true; 
    }

    void ShutDown() {}

    void Render() { 
        RenderSelector();
    }
} // namespace Walker::WalkerUi::Screens::Store
