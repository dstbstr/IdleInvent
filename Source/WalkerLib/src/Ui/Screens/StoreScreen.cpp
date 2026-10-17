#include "Walker/Ui/Screens/StoreScreen.h"
#include "Walker/WalkerSettings.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Ui/Selectors.h"
#include "Walker/Ui/UpgradeRow.h"

#include <Utilities/EnumUtils.h>

#include <imgui.h>
#include <algorithm>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    WalkerSettings* Settings{nullptr};
    VehicleKind SelectedKind{VehicleKind::Foot};

    void RenderStats(VehicleKind kind) {
        const auto& rates = Home->Rates;
		ImGui::Text("Cost: %s", rates.GetVehicleCost(kind).ToScientific().c_str());
		ImGui::Text("Max Speed: %s", rates.GetVehicleMaxSpeed(kind).ToScientific().c_str());
		ImGui::Text("Capacity: %s", rates.GetVehicleCapacity(kind).ToScientific().c_str());
		ImGui::Text("Base Acceleration: %s", rates.GetBaseAccel(kind).ToScientific().c_str());
		ImGui::Text("Powered Acceleration: %s", rates.GetPoweredAccel(kind).ToScientific().c_str());
		//ImGui::Text("Fuel Efficiency: %s", rates.GetFuelEfficiency(kind).ToScientific().c_str());
    }

    void RenderSelector() {
        WalkerUi::VehicleSelector("VehicleSelector", Enum::GetAllValues<VehicleKind>(), SelectedKind);

		const auto& rates = Home->Rates;
		auto cost = rates.GetVehicleCost(SelectedKind);

        ImGui::BeginDisabled(Home->Vehicles.Contains(SelectedKind) || !Home->Funds.CanAfford(cost));
        if(ImGui::Button("Purchase")) {
            if(Home->Funds.Spend(cost)) {
                if(!Home->Vehicles.Add(SelectedKind)) {
                    // refund
					Home->Funds.Add(cost);
                }
            }
        }
        ImGui::EndDisabled();

        ImGui::BeginDisabled(!Home->Vehicles.Contains(SelectedKind));
        ImGui::TextUnformatted("Acceleration");
        ImGui::SameLine();
		WalkerUi::RenderUpgradeRow("Acceleration", UpgradeKey::Vehicle(SelectedKind, VehicleStat::Accel), Home->Rates);

        ImGui::TextUnformatted("Max Speed");
		ImGui::SameLine();
		WalkerUi::RenderUpgradeRow("Max Speed", UpgradeKey::Vehicle(SelectedKind, VehicleStat::Speed), Home->Rates);

        ImGui::TextUnformatted("Capacity");
		ImGui::SameLine();
		WalkerUi::RenderUpgradeRow("Capacity", UpgradeKey::Vehicle(SelectedKind, VehicleStat::Capacity), Home->Rates);
        ImGui::EndDisabled();
    }
}

namespace Walker::WalkerUi::Screens::Store {
    bool Initialize() { 
		auto& services = ServiceLocator::Get();
		Home = &services.GetRequired<HomeBase>();
		Settings = &services.GetRequired<WalkerSettings>();
        return true; 
    }

    void ShutDown() {
        Home = nullptr;
        Settings = nullptr;
    }

    void Render() { 
        RenderSelector();
        RenderStats(SelectedKind);
    }
} // namespace Walker::WalkerUi::Screens::Store
