#include "Walker/Ui/Screens/TravelScreen.h"
#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Journey/Endpoints.h"

#include <DesignPatterns/PubSub.h>
#include <DesignPatterns/ServiceLocator.h>
#include <GameState/GameTime.h>
#include <Manage/TickManager.h>
#include <Math/BigInt.h>
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

    void DotSlider(f32 position) {
        auto origin = ImGui::GetCursorScreenPos();
        auto width = ImGui::GetContentRegionAvail().x;
        auto height = ImGui::GetFrameHeight();
        auto radius = ImGui::GetFontSize() * 0.15f;

        ImGui::Dummy({width, height});
        if(width > radius * 2.f) {
            auto start = ImVec2{origin.x + radius, origin.y + height * 0.5f};
            auto end = ImVec2{origin.x + width - radius, start.y};
            auto marker = ImVec2{start.x + (end.x - start.x) * position, start.y};

            auto* drawList = ImGui::GetWindowDrawList();
            drawList->AddLine(start, end, ImGui::GetColorU32(ImGuiCol_Separator), 2.f);
            drawList->AddCircleFilled(marker, radius, ImGui::GetColorU32(ImGuiCol_SliderGrabActive));
        };
    }

    const char* Str(Walker::Quantity q) {
        return q.ToHumanReadable(2, 3).value_or(q.ToScientific(2, 3)).c_str();
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

            if(phase == Phase::Preparing) {
                // load fuel
                if(ImGui::Button("Start")) {
                    CurrentJourney->Start();
                }
            } else if(phase == Phase::Outbound || phase == Phase::Returning) {
                if(ImGui::Button("Click")) {
                    CurrentJourney->Tick(OneSecond);
                }
            }
            auto ratio = CurrentJourney->GetJourneyRatio();
            DotSlider(ratio);
        } else {
            if(ImGui::Button("Start Journey")) {
                Services->Set<Journey>(*CurrentVehicle, EndpointKind::AcrossTheStreet);
                CurrentJourney = Services->Get<Journey>();
            }
        }

        auto dist = CurrentJourney ? CurrentJourney->GetCurrentDistance() : Distance{0};
        auto dest = CurrentJourney ? CurrentJourney->GetEndDistance() : Distance{0};
        auto accel = CurrentJourney ? CurrentJourney->GetCurrentAcceleration() : Acceleration{0};
        auto speed = CurrentJourney ? CurrentJourney->GetCurrentSpeed() : Speed{0};
        auto cargo = CurrentVehicle ? CurrentVehicle->FillRatio() : 0.f;

        ImGui::Text("CurrentDistance: %sm / %sm", Str(dist), Str(dest));
        ImGui::Text("Acceleration: %s m/s^2", Str(accel));
        ImGui::Text("Speed: %s m/s", Str(speed));

        ImGui::Text("Cargo Fill: %.1f%%", cargo * 100.f);
    }
}
