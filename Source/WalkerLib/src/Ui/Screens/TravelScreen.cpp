#include "Walker/Ui/Screens/TravelScreen.h"
#include "Walker/WalkerUnits.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Journey/Endpoints.h"
#include "Walker/Ui/VehicleSelector.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Manage/TickManager.h>
#include <Math/BigInt.h>
#include <Ui/UiUtil.h>
#include <Ui/Widgets/DotSlider.h>
#include <Ui/Widgets/StackedProgress.h>
#include <Utilities/Handle.h>

#include <imgui.h>

#include <algorithm>
#include <optional>
#include <vector>

namespace {
    using namespace Walker;
    Journey* CurrentJourney{nullptr};
    HomeBase* Home{nullptr};

    ServiceLocator* Services{nullptr};
    std::vector<ScopedHandle> Subs{};

    auto DeliveredColor = IM_COL32(0, 255, 0, 255);
    auto OnboardColor = IM_COL32(255, 255, 0, 255);
    auto AwaitingColor = IM_COL32(255, 0, 0, 255);

    auto CrewColor = IM_COL32(0, 255, 255, 255);
    auto FuelColor = IM_COL32(255, 0, 255, 255);
    auto CargoColor = IM_COL32(255, 255, 0, 255);

    std::string Str(Quantity q) {
        return q.ToHumanReadable(2, 3).value_or(q.ToScientific(2, 3));
    }

    void RenderPreparing() {
		auto& garage = Home->Vehicles;
		VehicleKind SelectedKind = garage.GetSelected()->Kind;
        if(WalkerUi::VehicleSelector("VehicleSelector", garage.GetAvailable(), SelectedKind)) {
			garage.Select(SelectedKind);
            CurrentJourney->ChangeVehicle(garage.GetSelected());
            Home->TravelingCrew = 1;
            garage.GetSelected()->SetCrew(1);
        }
        auto* vehicle = garage.GetSelected();

		auto maxFuel = std::max(Zero, vehicle->TotalCapacity - vehicle->CrewMass - vehicle->CargoMass);
        auto maxFuelPercent = vehicle->TotalCapacity > Zero
            ? std::clamp(static_cast<f32>(Mass::Ratio(maxFuel, vehicle->TotalCapacity)), 0.f, 1.f)
            : 0.f;
        f32 fuelPercent = vehicle->FuelMass > Zero
            ? static_cast<f32>(Mass::Ratio(vehicle->FuelMass, vehicle->TotalCapacity))
            : 0.f;
        ImGui::TextUnformatted("Fuel");
        Ui::DotSlider("FuelSlider", fuelPercent, "", "", 0, 0.f, maxFuelPercent);
        vehicle->FuelMass = std::min(maxFuel, vehicle->TotalCapacity * fuelPercent);

        ImGui::TextUnformatted("Crew");
        u64 crewCount = Home->TravelingCrew;
        ImGui::BeginDisabled(crewCount <= 1);
        if (ImGui::SmallButton("-")) {
            crewCount--;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(crewCount >= Home->TotalCrew || !vehicle->CanHoldMoreCrew());
        if (ImGui::SmallButton("+")) {
            crewCount++;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::Text("%llu/%llu", crewCount, Home->TotalCrew);

        vehicle->SetCrew(crewCount);
        Home->TravelingCrew = crewCount;

        if (ImGui::Button("Start")) {
            CurrentJourney->Start();
        }
    }

    void RenderTraveling() {
        if (ImGui::Button("Click")) {
            CurrentJourney->Tick(OneSecond);
        }
    }

    void RenderLoading(f32 ratio) {
        ImGui::ProgressBar(ratio, ImVec2{ 0.f, 0.f });
    }

    void RenderJourney() {
        auto phase = CurrentJourney->GetPhase();
        auto phaseStr = ToString(phase);
        ImGui::TextUnformatted(phaseStr.data(), phaseStr.data() + phaseStr.size());
        auto eta = CurrentJourney->GetPhaseEta();
        ImGui::SameLine();
        auto etaString = Time::ToTimeString(eta);
        ImGui::Text("[%s]", etaString.c_str());

        if (phase == Phase::Preparing) {
            RenderPreparing();
        }
        else if (phase == Phase::Outbound || phase == Phase::Returning) {
            RenderTraveling();
        }
        else {
            auto ratio = phase == Phase::Loading ? CurrentJourney->GetLoadingRatio() : CurrentJourney->GetUnloadRatio();
            RenderLoading(ratio);
            if(phase == Phase::Loading && ImGui::Button("Return Early")) {
                CurrentJourney->ReturnEarly();
            }
        }

        auto ratio = CurrentJourney->GetJourneyRatio();
        auto endpointStr = ToString(CurrentJourney->GetEndpoint()->Kind);
        ImGui::BeginDisabled();
        ImGui::PushFont(GetFont(FontSizes::H3));
        Ui::DotSlider("JourneySlider", ratio, "Home", endpointStr.c_str(), 9);
        ImGui::PopFont();
        ImGui::EndDisabled();
    }

    void RenderHome() {
        if (ImGui::Button("Start Journey")) {
            Services->Reset<Journey>();
            Services->Set<Journey>(Home->Vehicles.GetSelected(), Home->Endpoints[0].get(), *Home);
            CurrentJourney = Services->Get<Journey>();
        }
    }

    void RenderStats() {
		const auto* vehicle = Home->Vehicles.GetSelected();
		ImGui::Text("$%s", Str(Home->Funds.GetBalance()).c_str());
        auto dist = CurrentJourney ? CurrentJourney->GetCurrentDistance() : Distance{ 0 };
        auto dest = CurrentJourney ? CurrentJourney->GetEndDistance() : Distance{ 0 };
        auto accel = CurrentJourney ? CurrentJourney->GetCurrentAcceleration() : Acceleration{ 0 };
        auto speed = CurrentJourney ? CurrentJourney->GetCurrentSpeed() : Speed{ 0 };
        auto cargo = vehicle ? vehicle->FillRatio() : 0.f;

        ImGui::Text("CurrentDistance: %sm / %sm", Str(dist).c_str(), Str(dest).c_str());
        ImGui::Text("Acceleration: %s m/s^2", Str(accel).c_str());
        ImGui::Text("Speed: %s m/s", Str(speed).c_str());

        ImGui::Text("Cargo Fill: %.1f%%", cargo * 100.f);

        if (CurrentJourney && vehicle) {
            auto total = CurrentJourney->GetInitialCargo();
            auto Fraction = [&](const Mass& amount) -> f32 {
                return total > Zero ? static_cast<f32>(Mass::Ratio(amount, total)) : 0.f;
                };

            auto segments = std::array<Ui::ProgressSegment, 3>{ {
                {Fraction(CurrentJourney->GetDeliveredCargo()), DeliveredColor},
                {Fraction(vehicle->CargoMass), OnboardColor},
                {Fraction(CurrentJourney->GetEndpointCargo()), AwaitingColor}
            } };

            ImGui::TextUnformatted("Cargo Progress");
            Ui::MultiProgress(segments);

            total = vehicle->TotalCapacity;
            auto cargoSegments = std::array<Ui::ProgressSegment, 3>{
                {{Fraction(vehicle->CrewMass), CrewColor},
                 {Fraction(vehicle->FuelMass), FuelColor},
                 {Fraction(vehicle->CargoMass), CargoColor}}
            };

            ImGui::TextUnformatted("Vehicle Fill");
            Ui::MultiProgress(cargoSegments);
        }
    }

    void RenderContent() {
        if (CurrentJourney) {
            RenderJourney();
            if(CurrentJourney->GetPhase() == Phase::Complete) {
                Services->Reset<Journey>();
				CurrentJourney = nullptr;
            }
        }
        else {
            RenderHome();
        }

        RenderStats();
    }
}

namespace Walker::WalkerUi::Screens::Travel {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        Services = &services;
        CurrentJourney = services.Get<Journey>();
        Home = &services.GetRequired<HomeBase>();

        return true; 
    }

    void ShutDown() {
        Subs.clear();
    }


    void Render() {
        RenderContent();
    }
}
