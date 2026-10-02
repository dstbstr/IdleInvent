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
            auto Upgrade = [&](const char* label, u64& spent) {
                ImGui::TableNextColumn();
                ImGui::PushID(label);
                auto startX = ImGui::GetCursorPosX();
				auto width = ImGui::GetContentRegionAvail().x;

                auto toSpend = GetPurchaseCount(points, Settings->PurchaseSetting);
                ImGui::BeginDisabled(toSpend == 0);
				if (ImGui::Button(label, ImVec2{ width, ImGui::GetFrameHeight() * 1.5f }) && points > 0) {
					spent += toSpend;
					points -= toSpend;
				}
                ImGui::EndDisabled();

				auto text = "x" + std::to_string(spent);
				auto offset = std::max(0.f, (width - ImGui::CalcTextSize(text.c_str()).x) * 0.5f);
                ImGui::SetCursorPosX(startX + offset);
                ImGui::TextUnformatted(text.c_str());
                ImGui::PopID();
            };

            Upgrade("Cargo", rebirth.CargoWorkPoints);
			Upgrade("Jobs", rebirth.JobWorkPoints);
			Upgrade("Accel", rebirth.AccelPoints);
			Upgrade("Speed", rebirth.MaxSpeedPoints);
			Upgrade("Capacity", rebirth.MaxCapacityPoints);

            ImGui::EndTable();
        }

		auto& prestige = Home->Rates.GetPrestiege();
		auto toSpend = GetPurchaseCount(prestige.AvailablePoints, Settings->PurchaseSetting);
        ImGui::BeginDisabled(toSpend == 0);
        if(ImGui::Button("Cargo##Prestiege") && toSpend > 0) {
			prestige.AvailablePoints -= toSpend;
			prestige.CargoWorkPoints += toSpend;
        }
        ImGui::EndDisabled();

		auto exponent = 1.0 + static_cast<f64>(prestige.CargoWorkPoints) * 0.1;
        ImGui::Text("^%.1f", exponent);

        ImGui::PopFont();
    }

    void RenderRebirth() {
        if(Home->FurthestEndpoint < EndpointKind::SolarSystem) {
			CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to rebirth?");
        ImGui::TextUnformatted("This will reset the following:");
        ImGui::TextUnformatted("- Vehicles");
        ImGui::TextUnformatted("- Money");
        ImGui::TextUnformatted("- Endpoints");
        ImGui::Separator();
		auto overage = static_cast<u64>(Home->FurthestEndpoint) - static_cast<u64>(EndpointKind::SolarSystem);
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
        if (Home->FurthestEndpoint < EndpointKind::FarGalaxy) {
            CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to Prestiege?");
        ImGui::TextUnformatted("This will reset the following in addition to rebirth:");
        ImGui::TextUnformatted("- Population");
        ImGui::TextUnformatted("- Tech");
        ImGui::TextUnformatted("- Rebirths");
        ImGui::Separator();
        auto overage = static_cast<u64>(Home->FurthestEndpoint) - static_cast<u64>(EndpointKind::FarGalaxy);
        auto prestigePoints = static_cast<u64>(std::pow(2, overage));
        ImGui::Text("In exchange you'll receive %llu prestige points", prestigePoints);

        if (ImGui::Button("Confirm") && prestigePoints > 0) {
            ServiceLocator::Get().Reset<Journey>();
            Home->Prestiege();
            Home->Rates.GetPrestiege().AvailablePoints += prestigePoints;
            CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }
    void RenderAscend() {
        if (Home->FurthestEndpoint < EndpointKind::GreatBeyond) {
            CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to Ascend?");
        ImGui::TextUnformatted("This will reset basically everything");
        ImGui::Separator();
        ImGui::TextUnformatted("In exchange you'll receive 1 ascend point");

        if (ImGui::Button("Confirm")) {
            ServiceLocator::Get().Reset<Journey>();
            Home->Ascend();
            Home->Rates.GetAscend().AvailablePoints++;
            CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }

	void RenderControls() {
        auto canRebirth = Home->FurthestEndpoint >= EndpointKind::SolarSystem;
        ImGui::BeginDisabled(!canRebirth);
        if(ImGui::Button("Rebirth")) {
			CurrentRebirthType = RebirthType::Rebirth;
		}
        ImGui::EndDisabled();
        
		auto canPrestiege = Home->FurthestEndpoint >= EndpointKind::FarGalaxy;
		ImGui::BeginDisabled(!canPrestiege);
		if (ImGui::Button("Prestiege")) {
			CurrentRebirthType = RebirthType::Prestiege;
		}
        ImGui::EndDisabled();

		auto canAscend = Home->FurthestEndpoint >= EndpointKind::GreatBeyond;
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
