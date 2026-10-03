#include "Walker/Ui/Screens/RebirthScreen.h"
#include "Walker/WalkerSettings.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>

#include <imgui.h>

#include <algorithm>
#include <format>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};
    WalkerSettings* Settings{nullptr};

    enum struct RebirthType { Rebirth, Prestiege, Ascend };
	std::optional<RebirthType> CurrentRebirthType{ std::nullopt };

    void RenderProgressionStore(const char* title, WalkerProgression& progression, auto pointFormat) {
		auto& points = progression.AvailablePoints;
		ImGui::Text("Available %s Points: %llu", title, points);
        ImGui::PushFont(GetFont(FontSizes::H3));
		auto minCellWidth = ImGui::GetFontSize() * 8.f;
		auto columns = ImGui::GetContentRegionAvail().x >= minCellWidth * 4.f ? 4 : 2;

        ImGui::PushID(title);
		if (ImGui::BeginTable("ProgressionUpgrades", columns, ImGuiTableFlags_SizingStretchSame)) {
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
				auto text = pointFormat(spent);
                auto offset = std::max(0.f, (width - ImGui::CalcTextSize(text.c_str()).x) * 0.5f);
                ImGui::SetCursorPosX(startX + offset);
                ImGui::TextUnformatted(text.c_str());
                ImGui::PopID();
            };
            Upgrade("Cargo", progression.CargoWorkPoints);
            Upgrade("Jobs", progression.JobWorkPoints);
            Upgrade("Accel", progression.AccelPoints);
            Upgrade("Speed", progression.MaxSpeedPoints);
            Upgrade("Capacity", progression.MaxCapacityPoints);
            ImGui::EndTable();
        }
        ImGui::PopID();
        ImGui::PopFont();
    }

    void RenderStore() {
		RenderProgressionStore("Rebirth", Home->Rates.GetRebirth(), [](u64 spent) { 
            return std::format("x{}", spent + 1); 
        });
		RenderProgressionStore("Prestiege", Home->Rates.GetPrestiege(), [](u64 spent) {
			auto exponent = 1.0 + static_cast<f64>(spent) * 0.1;
			return std::format("^{:.1f}", exponent);
		});
		RenderProgressionStore("Ascend", Home->Rates.GetAscend(), [](u64 spent) {
			return std::format("+{}", spent);
		});
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

    void RenderTotalBonuses() {
		auto formatBonus = [](const std::pair<Quantity, double>& bonus) {
			return std::format("{}^{:.1f}", bonus.first.ToHumanReadable(2).value_or(bonus.first.ToScientific(2)), bonus.second);
		};
		ImGui::Text("Cargo Loading: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::CargoWorkPoints)).c_str());
		ImGui::Text("Job Work: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::JobWorkPoints)).c_str());
		ImGui::Text("Acceleration: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::AccelPoints)).c_str());
		ImGui::Text("Max Speed: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::MaxSpeedPoints)).c_str());
		ImGui::Text("Max Capacity: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::MaxCapacityPoints)).c_str());
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
        RenderTotalBonuses();
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
