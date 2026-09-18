#include "Walker/Ui/Screens/TravelScreen.h"
#include "Walker/WalkerUnits.h"
#include "Walker/Journey/Journey.h"
#include "Walker/Journey/Endpoints.h"

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
    std::optional<Walker::Journey> CurrentJourney{};
    Walker::OwnedVehicle CurrentVehicle{Walker::VehicleKind::Car};

    std::vector<ScopedHandle> Subs{};

    void DotSlider(f32 position) {
        auto origin = ImGui::GetCursorScreenPos();
        auto width = ImGui::GetContentRegionAvail().x;
        auto height = ImGui::GetFrameHeight();
        auto radius = ImGui::GetFontSize() * 0.25f;

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
}

namespace Walker::WalkerUi::Screens::Travel {
    bool Initialize() { 
        TickManager::Get().Register(Subs, [](BaseTime elapsed) {
            if(CurrentJourney.has_value()) {
                CurrentJourney->Tick(elapsed);
            }
        });

        return true; 
    }

    void ShutDown() {
        Subs.clear();
    }

    void Render() {
        if(CurrentJourney) {
            auto ratio = CurrentJourney->GetJourneyRatio();
            DotSlider(ratio);
        } else {
            if(ImGui::Button("Start Journey")) {
                CurrentJourney.emplace(CurrentVehicle, EndpointKind::AcrossTown);
                CurrentJourney->Start();
            }
        }

        auto dist = CurrentJourney ? CurrentJourney->GetCurrentDistance() : Distance{0};
        auto dest = CurrentJourney ? CurrentJourney->GetEndDistance() : Distance{0};
        auto accel = CurrentJourney ? CurrentJourney->GetCurrentAcceleration() : Acceleration{0};

        ImGui::Text("CurrentDistance: %sm / %sm", 
            dist.ToHumanReadable().value_or(dist.ToScientific()).c_str(),
            dest.ToHumanReadable().value_or(dest.ToScientific()).c_str());
        ImGui::Text("Acceleration: %sm/s^2", accel.ToHumanReadable().value_or(accel.ToScientific()).c_str());
    }
}
