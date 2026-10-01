#include "Walker/Ui/Screens/RebirthScreen.h"
#include "Walker/WalkerSettings.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>

#include <imgui.h>

#include <algorithm>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    WalkerSettings* Settings{nullptr};

    enum struct RebirthType { Rebirth, Prestiege, Ascend };
	std::optional<RebirthType> CurrentRebirthType{ std::nullopt };

    void RenderStore() {
        auto& rebirth = Home->Rates.GetRebirth();
		auto& points = rebirth.AvailablePoints;
		ImGui::Text("Available Rebirth Points: %llu", points);
        ImGui::PushFont(GetFont(FontSizes::H3));
        auto minCellWidth = ImGui::GetFontSize() * 8.f;
        auto columns = ImGui::GetContentRegionAvail().x >= minCellWidth * 4.f ? 4 : 2;

        if(ImGui::BeginTable("RebirthUpgrades", columns, ImGuiTableFlags_SizingStretchSame)) {
            auto Upgrade = [&](const char* label, Quantity& multiplier) {
                ImGui::TableNextColumn();
                ImGui::PushID(label);
                auto startX = ImGui::GetCursorPosX();
				auto width = ImGui::GetContentRegionAvail().x;

                ImGui::BeginDisabled(points == 0);
				if (ImGui::Button(label, ImVec2{ width, ImGui::GetFrameHeight() * 1.5f }) && points > 0) {
					multiplier += 1;
					points--;
				}
                ImGui::EndDisabled();

				auto text = "x" + multiplier.ToHumanReadable(0).value_or(multiplier.ToScientific(0));
				auto offset = std::max(0.f, (width - ImGui::CalcTextSize(text.c_str()).x) * 0.5f);
                ImGui::SetCursorPosX(startX + offset);
                ImGui::TextUnformatted(text.c_str());
                ImGui::PopID();
            };

            Upgrade("Cargo", rebirth.CargoWorkRateMultiplier);
			Upgrade("Jobs", rebirth.JobWorkRateMultiplier);
			Upgrade("Speed", rebirth.MaxSpeedMultiplier);
			Upgrade("Capacity", rebirth.MaxCapacityMultiplier);

            ImGui::EndTable();
        }

        ImGui::PopFont();
    }

    void RenderRebirth() {
        if(Home->GetMaxScoutKind() < EndpointKind::MilkyWay) {
			CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to rebirth?");
        ImGui::TextUnformatted("This will reset the following:");
        ImGui::TextUnformatted("- Vehicles");
        ImGui::TextUnformatted("- Money");
        ImGui::TextUnformatted("- Upgrades");
        ImGui::Separator();
		auto overage = static_cast<u64>(Home->GetMaxScoutKind()) - static_cast<u64>(EndpointKind::MilkyWay);
		auto rebirthPoints = static_cast<u64>(std::pow(2, overage));
        ImGui::Text("In exchange you'll receive %llu rebirth points", rebirthPoints);

        if(ImGui::Button("Confirm") && rebirthPoints > 0) {
            ServiceLocator::Get().Reset<Journey>();
            Home->Rebirth();
            Home->Rates.GetRebirth().AvailablePoints += rebirthPoints;
			CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
		if (ImGui::Button("Back")) {
			CurrentRebirthType = std::nullopt;
		}
    }
    void RenderPrestiege() {
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }
    void RenderAscend() {
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }

	void RenderControls() {
        auto canRebirth = Home->GetMaxScoutKind() >= EndpointKind::MilkyWay;
        ImGui::BeginDisabled(!canRebirth);
        if(ImGui::Button("Rebirth")) {
			CurrentRebirthType = RebirthType::Rebirth;
		}
        ImGui::EndDisabled();
        
		auto canPrestiege = Home->GetMaxScoutKind() >= EndpointKind::EdgeOfUniverse;
		ImGui::BeginDisabled(!canPrestiege);
		if (ImGui::Button("Prestiege")) {
			CurrentRebirthType = RebirthType::Prestiege;
		}
        ImGui::EndDisabled();

		auto canAscend = Home->GetMaxScoutKind() >= EndpointKind::GreatBeyond;
		ImGui::BeginDisabled(!canAscend);
		if (ImGui::Button("Ascend")) {
			CurrentRebirthType = RebirthType::Ascend;
        }
        ImGui::EndDisabled();
	}

    void RenderContent() {
        RenderStore();

        if(CurrentRebirthType) {
			switch (*CurrentRebirthType) {
                using enum RebirthType;
			    case Rebirth: RenderRebirth(); break;
			    case Prestiege: RenderPrestiege(); break;
			    case Ascend: RenderAscend(); break;
			}
        } else {
            RenderControls();
        }
    }
}

namespace Walker::WalkerUi::Screens::Rebirth {
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
        RenderContent();
    }
} // namespace Walker::WalkerUi::Screens::Crew
