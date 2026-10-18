#include "Walker/Ui/Screens/TravelScreen.h"
#include "Walker/WalkerSettings.h"
#include "Walker/WalkerUnits.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Ui/Selectors.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Manage/TickManager.h>
#include <Math/BigInt.h>
#include <Ui/UiUtil.h>
#include <Ui/Widgets/DotSlider.h>
#include <Ui/Widgets/StackedProgress.h>
#include <Utilities/EnumUtils.h>
#include <Utilities/Handle.h>

#include <imgui.h>

#include <algorithm>
#include <optional>
#include <vector>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    WalkerSettings* Settings{nullptr};

    ServiceLocator* Services{nullptr};

    auto DeliveredColor = IM_COL32(0, 255, 0, 255);
    auto OnboardColor = IM_COL32(255, 255, 0, 255);
    auto AwaitingColor = IM_COL32(255, 0, 0, 255);

    auto CrewColor = IM_COL32(0, 255, 255, 255);
    auto FuelColor = IM_COL32(255, 0, 255, 255);
    auto CargoColor = IM_COL32(255, 255, 0, 255);

    std::string Str(Quantity q) {
        return q.ToHumanReadable(2, 3).value_or(q.ToScientific(2, 3));
    }

    void RenderGarage(Journey& journey) {
		auto& garage = Home->Vehicles;
        VehicleKind SelectedKind = garage.GetSelected()->Kind;
        if (WalkerUi::VehicleSelector("VehicleSelector", garage.GetAvailable(), SelectedKind)) {
            garage.Select(SelectedKind);
            journey.ChangeVehicle(*garage.GetSelected());
            Home->Crew.ClearTravelers();
            garage.GetSelected()->SetCrew(1);
        }
    }

    void RenderDestination(Journey& journey) {
        static auto kinds = Enum::GetAllValues<EndpointKind>();
        auto selected = journey.GetNextScout();
        auto count = static_cast<size_t>(Home->GetMaxScoutKind()) - static_cast<size_t>(EndpointKind::Neighborhood) + 1;

        auto choices = std::span<const EndpointKind>(kinds).first(count);

        if (WalkerUi::ScoutSelector("ScoutSelector", choices, selected)) {
            journey.SetNextScout(selected);
        }
    }

    void RenderFuel(Journey& journey) {
		auto* vehicle = Home->Vehicles.GetSelected();
		auto total = vehicle->GetTotalCapacity(Home->Rates);
        auto maxFuel = std::max(Zero, total - vehicle->CrewMass - vehicle->CargoMass);
        auto maxFuelPercent = total > Zero
            ? std::clamp(static_cast<f32>(Mass::Ratio(maxFuel, total)), 0.f, 1.f)
            : 0.f;
        f32 fuelPercent = vehicle->FuelMass > Zero
            ? static_cast<f32>(Mass::Ratio(vehicle->FuelMass, total))
            : 0.f;
        ImGui::TextUnformatted("Fuel");
        Ui::DotSlider("FuelSlider", fuelPercent, "", "", 0, 0.f, maxFuelPercent);
        vehicle->FuelMass = std::min(maxFuel, total * fuelPercent);
    }

    void RenderCrew(Journey& journey) {
        auto& crew = Home->Crew;
		auto* vehicle = Home->Vehicles.GetSelected();

		auto removable = GetPurchaseCount(crew[CrewRole::Traveling] - 1, Settings->PurchaseSetting);
        ImGui::BeginDisabled(removable == 0);
        if (ImGui::SmallButton("-") && crew.TryUnassign(removable, CrewRole::Traveling)) {
            vehicle->SetCrew(crew[CrewRole::Traveling]);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();

        auto available = crew[CrewRole::Idle];
		auto capacity = vehicle->GetRemainingCrewCapacity(Home->Rates).TryConvert<u64>().value_or(available);
		available = std::min(available, capacity);
		auto addable = GetPurchaseCount(available, Settings->PurchaseSetting);
        ImGui::BeginDisabled(addable == 0);
        if (ImGui::SmallButton("+") && crew.TryAssign(addable, CrewRole::Traveling)) {
            vehicle->SetCrew(crew[CrewRole::Traveling]);
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Text("Aboard: %llu | Idle: %llu | Stationed: %llu",
            crew[CrewRole::Traveling],
            crew[CrewRole::Idle],
            crew[CrewRole::Stationed]);
    }

    void RenderPreparing(Journey& journey) {
        RenderGarage(journey);
        RenderDestination(journey);
        RenderFuel(journey);
        RenderCrew(journey);

        if (ImGui::Button("Start") && journey.GetEndpoint() && journey.GetVehicleKind() != VehicleKind::Unset) {
            journey.Start();
        }
    }

    void RenderTraveling(Journey& journey) {
        if (ImGui::Button("Click")) {
            journey.Tick(OneSecond);
        }
    }

    void RenderLoading(f32 ratio) {
        ImGui::ProgressBar(ratio, ImVec2{ 0.f, 0.f });
    }

    void RenderJourney(Journey& journey) {
        auto phase = journey.GetPhase();

		ImGui::Text("Vechicle: %s", ToString(journey.GetVehicleKind()).c_str());
        ImGui::SameLine();
        ImGui::BeginDisabled(phase != Phase::Preparing && phase != Phase::Scouting);
        if(ImGui::Button("Trip setup")) {
            ImGui::OpenPopup("TripSetup");
        }
        ImGui::EndDisabled();

        auto popupWidth = std::min(480.f, ImGui::GetIO().DisplaySize.x * 0.9f);
        ImGui::SetNextWindowSize(ImVec2{popupWidth, 0.f}, ImGuiCond_Appearing);
        if(ImGui::BeginPopup("TripSetup")) {
            if(phase != Phase::Preparing && phase != Phase::Scouting) {
                ImGui::CloseCurrentPopup();
            } else {
                RenderGarage(journey);
                RenderDestination(journey);
                RenderFuel(journey);
                RenderCrew(journey);

                if(ImGui::Button("Done")) {
					ImGui::CloseCurrentPopup();
                }
            }
            ImGui::EndPopup();
        }

        auto* endpoint = journey.GetEndpoint();
        ImGui::TextUnformatted(endpoint ? endpoint->Name.c_str() : "Scouting...");

        auto phaseName = ToString(phase);
		ImGui::TextUnformatted(phaseName.data(), phaseName.data() + phaseName.size());
        ImGui::SameLine();
		if (phase == Phase::Preparing && !Home->Tech[TechKind::Preparation].Researched) {
			ImGui::Text("- waiting for departure");
		} else {
			auto eta = Time::ToTimeString(journey.GetPhaseEta());
			ImGui::Text("[%s]", eta.c_str());
		}

        auto position = journey.GetJourneyRatio();
        ImGui::BeginDisabled();
		Ui::DotSlider("JourneySlider", position, "Home", endpoint ? endpoint->Name.c_str() : "??", 9);
        ImGui::EndDisabled();

        const char* action = "Complete";
        switch(phase) {
            using enum Phase;
            case Scouting: action = "Help Scout"; break;
            case Preparing: action = "Depart"; break;
            case Outbound:
            case Returning: action = "Help Travel"; break;
            case Loading:
            case Unloading: action = "Move Cargo"; break;
            default: break;
        }

        auto buttonLabel = std::string{action} + "###PrimaryAction";
        ImGui::BeginDisabled(phase == Phase::Complete);
        if(ImGui::Button(buttonLabel.c_str(), ImVec2{-1.f, ImGui::GetFrameHeight() * 4.f})) {
			phase == Phase::Preparing 
                ? journey.Start()
				: journey.Tick(OneSecond);
        }
        ImGui::EndDisabled();
        ImGui::BeginDisabled(journey.GetPhase() != Phase::Loading);
        if(ImGui::Button("Return early")) {
            journey.ReturnEarly();
        }
        ImGui::EndDisabled();

        ImGui::Text("Delivered: %.0f%% | Stationed here: %llu", 
            journey.GetDeliveryRatio() * 100.f, 
            endpoint ? endpoint->StationedCrew : 0);

		if (endpoint && endpoint->RemainingCargo == Zero && endpoint->StationedCrew > 0) {
			ImGui::Text("%llu stationed crew awaiting pickup", endpoint->StationedCrew);
		} else {
            ImGui::Dummy(ImVec2{0.f, ImGui::GetTextLineHeight()});
        }
    }

    void RenderStats(Journey& journey) {
		const auto* vehicle = Home->Vehicles.GetSelected();
        auto dist = journey.GetCurrentDistance();
        auto dest = journey.GetEndDistance();
        auto accel = journey.GetCurrentAcceleration();
        auto speed = journey.GetCurrentSpeed();
        auto cargo = vehicle ? vehicle->FillRatio(Home->Rates) : 0.f;

        ImGui::Text("CurrentDistance: %sm / %sm", Str(dist).c_str(), Str(dest).c_str());
        ImGui::Text("Acceleration: %s m/s^2", Str(accel).c_str());
        ImGui::Text("Speed: %s m/s", Str(speed).c_str());

        ImGui::Text("Cargo Fill: %.1f%%", cargo * 100.f);

        if (vehicle) {
            auto total = journey.GetInitialCargo();
            auto Fraction = [&](const Mass& amount) -> f32 {
                return total > Zero ? static_cast<f32>(Mass::Ratio(amount, total)) : 0.f;
                };

            auto segments = std::array<Ui::ProgressSegment, 3>{ {
                {Fraction(journey.GetDeliveredCargo()), DeliveredColor},
                {Fraction(vehicle->CargoMass), OnboardColor},
                {Fraction(journey.GetEndpointCargo()), AwaitingColor}
            } };

            ImGui::TextUnformatted("Cargo Progress");
            Ui::MultiProgress(segments);

            total = vehicle->GetTotalCapacity(Home->Rates);
            auto cargoSegments = std::array<Ui::ProgressSegment, 3>{
                {{Fraction(vehicle->CrewMass), CrewColor},
                 {Fraction(vehicle->FuelMass), FuelColor},
                 {Fraction(vehicle->CargoMass), CargoColor}}
            };

            ImGui::TextUnformatted("Vehicle Fill");
            Ui::MultiProgress(cargoSegments);
        }
    }

    void RenderContent(Journey& journey) {
		ImGui::Text("$%s", Str(Home->Funds.GetBalance()).c_str());
        RenderJourney(journey);

        if(ImGui::CollapsingHeader("Stats")) {
            RenderStats(journey);
        }
    }
}

namespace Walker::WalkerUi::Screens::Travel {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        Services = &services;
        Home = &services.GetRequired<HomeBase>();
		Settings = &services.GetRequired<WalkerSettings>();
        return true; 
    }

    void ShutDown() {
        Services = nullptr;
        Home = nullptr;
        Settings = nullptr;
    }

    void Render() {
        RenderContent(Home->GetJourney());
    }
}
