#include "Walker/Ui/Screens/TravelScreen.h"
#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Journey/Endpoints.h"

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
    Walker::Journey* CurrentJourney{nullptr};
    Walker::OwnedVehicle* CurrentVehicle{nullptr};

    ServiceLocator* Services{nullptr};
    std::vector<ScopedHandle> Subs{};

    auto DeliveredColor = IM_COL32(0, 255, 0, 255);
    auto OnboardColor = IM_COL32(255, 255, 0, 255);
    auto AwaitingColor = IM_COL32(255, 0, 0, 255);

    auto CrewColor = IM_COL32(0, 255, 255, 255);
    auto FuelColor = IM_COL32(255, 0, 255, 255);
    auto CargoColor = IM_COL32(255, 255, 0, 255);

    std::string Str(Walker::Quantity q) {
        return q.ToHumanReadable(2, 3).value_or(q.ToScientific(2, 3));
    }
}

namespace Walker::WalkerUi::Screens::Travel {
    bool Initialize() {
        auto& services = ServiceLocator::Get();
        Services = &services;
        CurrentJourney = services.Get<Journey>();
        CurrentVehicle = &services.GetRequired<OwnedVehicle>();

        services.GetRequired<PubSub<VehicleChanged>>().Subscribe(Subs, [](const auto&) {
            CurrentVehicle = ServiceLocator::Get().Get<OwnedVehicle>();
        });

        return true; 
    }

    void ShutDown() {
        Subs.clear();
    }

    void Render() {
        if(CurrentJourney) {
            auto phase = CurrentJourney->GetPhase();
            auto phaseStr = ToString(phase);
            ImGui::TextUnformatted(phaseStr.data(), phaseStr.data() + phaseStr.size());
            auto eta = CurrentJourney->GetPhaseEta();
            ImGui::SameLine();
            auto etaString = Time::ToTimeString(eta);
            ImGui::Text("[%s]", etaString.c_str());

            if(phase == Phase::Preparing) {
                f32 fuelPercent = CurrentVehicle->FuelMass > Zero
                    ? static_cast<f32>(Mass::Ratio(CurrentVehicle->FuelMass, CurrentVehicle->TotalCapacity))
                    : 0.f;
                Ui::DotSlider("FuelSlider", fuelPercent);
                CurrentVehicle->FuelMass = CurrentVehicle->TotalCapacity * fuelPercent;

                if(ImGui::Button("Start")) {
                    CurrentJourney->Start();
                }
            } else if(phase == Phase::Outbound || phase == Phase::Returning) {
                if(ImGui::Button("Click")) {
                    CurrentJourney->Tick(OneSecond);
                }
            } else if(phase == Phase::Loading) {
                ImGui::ProgressBar(CurrentJourney->GetLoadingRatio(), ImVec2{0.f, 0.f});
            } else if(phase == Phase::Unloading) {
                ImGui::ProgressBar(CurrentJourney->GetUnloadRatio(), ImVec2{0.f, 0.f});
            }
            auto ratio = CurrentJourney->GetJourneyRatio();
            auto endpointStr = ToString(CurrentJourney->GetEndpoint());
            ImGui::BeginDisabled();
            ImGui::PushFont(GetFont(FontSizes::H3));
            Ui::DotSlider("JourneySlider", ratio, "Home", endpointStr.c_str(), 9);
            ImGui::PopFont();
            ImGui::EndDisabled();

        } else {
            if(ImGui::Button("Start Journey")) {
                Services->Reset<Journey>();
                Services->Set<Journey>(*CurrentVehicle, EndpointKind::AcrossTheStreet);
                CurrentJourney = Services->Get<Journey>();
            }
        }

        auto dist = CurrentJourney ? CurrentJourney->GetCurrentDistance() : Distance{0};
        auto dest = CurrentJourney ? CurrentJourney->GetEndDistance() : Distance{0};
        auto accel = CurrentJourney ? CurrentJourney->GetCurrentAcceleration() : Acceleration{0};
        auto speed = CurrentJourney ? CurrentJourney->GetCurrentSpeed() : Speed{0};
        auto cargo = CurrentVehicle ? CurrentVehicle->FillRatio() : 0.f;

        ImGui::Text("CurrentDistance: %sm / %sm", Str(dist).c_str(), Str(dest).c_str());
        ImGui::Text("Acceleration: %s m/s^2", Str(accel).c_str());
        ImGui::Text("Speed: %s m/s", Str(speed).c_str());

        ImGui::Text("Cargo Fill: %.1f%%", cargo * 100.f);

        if(CurrentJourney && CurrentVehicle) {
            auto total = CurrentJourney->GetInitialCargo();
            auto Fraction = [&](const Mass& amount) -> f32 {
                return total > Zero ? static_cast<f32>(Mass::Ratio(amount, total)) : 0.f;
            };
            
            auto segments = std::array<Ui::ProgressSegment, 3>{{
                {Fraction(CurrentJourney->GetDeliveredCargo()), DeliveredColor},
                {Fraction(CurrentVehicle->CargoMass), OnboardColor},
                {Fraction(CurrentJourney->GetEndpointCargo()), AwaitingColor}
            }};

            ImGui::TextUnformatted("Cargo Progress");
            Ui::MultiProgress(segments);

            total = CurrentVehicle->TotalCapacity;
            auto cargoSegments = std::array<Ui::ProgressSegment, 3>{
                {{Fraction(CurrentVehicle->CrewMass), CrewColor},
                 {Fraction(CurrentVehicle->FuelMass), FuelColor},
                 {Fraction(CurrentVehicle->CargoMass), CargoColor}}
            };

            ImGui::TextUnformatted("Vehicle Fill");
            Ui::MultiProgress(cargoSegments);
        }

    }
}
