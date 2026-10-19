#include "Walker/Ui/Screens/RebirthScreen.h"
#include "Walker/Ui/UpgradeRow.h"
#include "Walker/WalkerSettings.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Journey/Journey.h"

#include <DesignPatterns/ServiceLocator.h>
#include <Ui/UiUtil.h>
#include <Utilities/EnumUtils.h>

#include <imgui.h>

#include <algorithm>
#include <format>
#include <limits>

namespace {
    using namespace Walker;
    HomeBase* Home{nullptr};

    enum struct RebirthType { Rebirth, Prestige, Ascend };
	std::optional<RebirthType> CurrentRebirthType{ std::nullopt };

    void RenderStore() {
        ImGui::PushFont(GetFont(FontSizes::H3));
        ImGui::SeparatorText("Global Upgrades");

        auto Upgrade = [&](const char* label, GlobalStat stat) {
            ImGui::TextUnformatted(label);
			WalkerUi::RenderUpgradeRow(label, UpgradeKey::Global(stat), Home->Rates);
        };

		Upgrade("Cargo Work", GlobalStat::CargoWork);
		Upgrade("All Jobs", GlobalStat::Jobs);
		Upgrade("Discovery", GlobalStat::Discovery);
		Upgrade("All Acceleration", GlobalStat::Accel);
		Upgrade("All Aerodynamics", GlobalStat::Drag);
		Upgrade("All Capacity", GlobalStat::Capacity);

        ImGui::PopFont();
    }

    void RenderRebirth() {
        auto rebirthPoints = Home->Rates.GetRebirthReward(Home->Stats.SinceRebirth().TotalDistance);
        if(rebirthPoints == 0) {
            CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to rebirth?");
        ImGui::TextUnformatted("This will reset the following:");
        ImGui::TextUnformatted("- Vehicles");
        ImGui::TextUnformatted("- Money");
        ImGui::TextUnformatted("- Endpoints");
        ImGui::Separator();
        ImGui::Text("In exchange you'll receive %llu rebirth points", rebirthPoints);

        if(ImGui::Button("Confirm") && Home->TryRebirth()) {
			CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
		if (ImGui::Button("Back")) {
			CurrentRebirthType = std::nullopt;
		}
    }

    void RenderPrestige() {
        auto prestigePoints = Home->Rates.GetPrestigeReward(Home->Stats.SincePrestige().TotalDistance);

        if (prestigePoints == 0) {
            CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to Prestige?");
        ImGui::TextUnformatted("This will reset the following in addition to rebirth:");
        ImGui::TextUnformatted("- Population");
        ImGui::TextUnformatted("- Tech");
        ImGui::TextUnformatted("- Rebirths");
        ImGui::Separator();
        ImGui::Text("In exchange you'll receive %llu prestige points", prestigePoints);

        if (ImGui::Button("Confirm") && Home->TryPrestige()) {
            CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }

    void RenderAscend() {
        auto ascendPoints = Home->Rates.GetAscendReward(Home->Stats.SinceAscend().TotalDistance);

        if (ascendPoints == 0) {
            CurrentRebirthType = std::nullopt;
            return;
        }

        ImGui::TextUnformatted("Would you like to Ascend?");
        ImGui::TextUnformatted("This will reset basically everything");
        ImGui::Separator();
        ImGui::Text("In exchange you'll receive %llu ascend points", ascendPoints);

        if (ImGui::Button("Confirm") && Home->TryAscend()) {
            CurrentRebirthType = std::nullopt;
        }
        ImGui::SameLine();
        if (ImGui::Button("Back")) {
            CurrentRebirthType = std::nullopt;
        }
    }

	void RenderControls() {
        auto rebirthPoints = Home->Rates.GetRebirthReward(Home->Stats.SinceRebirth().TotalDistance);
        ImGui::BeginDisabled(rebirthPoints == 0);
        if(ImGui::Button("Rebirth")) {
			CurrentRebirthType = RebirthType::Rebirth;
		}
        ImGui::EndDisabled();
        
		auto PrestigePoints = Home->Rates.GetPrestigeReward(Home->Stats.SincePrestige().TotalDistance);
		ImGui::BeginDisabled(PrestigePoints == 0);
		if (ImGui::Button("Prestige")) {
			CurrentRebirthType = RebirthType::Prestige;
		}
        ImGui::EndDisabled();

		auto ascendPoints = Home->Rates.GetAscendReward(Home->Stats.SinceAscend().TotalDistance);
		ImGui::BeginDisabled(ascendPoints == 0);
		if (ImGui::Button("Ascend")) {
			CurrentRebirthType = RebirthType::Ascend;
        }
        ImGui::EndDisabled();
	}

    void RenderTotalBonuses() {
		auto formatBonus = [](const RateBonus& bonus) {
			return std::format("(({} * {:.2f}) ^ {:.2f}) * {:.2f}", bonus.PreMul, bonus.PreMul, bonus.Exponent, bonus.PostMul);
		};
		ImGui::PushFont(GetFont(FontSizes::H3));
        // TODO: Consider a better way to show this
        /*
		ImGui::Text("Cargo Loading: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::CargoWorkPoints)).c_str());
		ImGui::Text("Job Work: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::JobWorkPoints)).c_str());
		ImGui::Text("Acceleration: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::AccelPoints)).c_str());
		ImGui::Text("Max Speed: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::MaxSpeedPoints)).c_str());
		ImGui::Text("Max Capacity: %s", formatBonus(Home->Rates.GetBonus(&WalkerProgression::MaxCapacityPoints)).c_str());
        */
        ImGui::PopFont();
    }

    void RenderContent() {

        if(CurrentRebirthType) {
			switch (*CurrentRebirthType) {
                using enum RebirthType;
			    case Rebirth: RenderRebirth(); break;
			    case Prestige: RenderPrestige(); break;
			    case Ascend: RenderAscend(); break;
			}
        } else {
            RenderStore();
            RenderControls();
            RenderTotalBonuses();
        }
    }
}

namespace Walker::WalkerUi::Screens::Rebirth {
    bool Initialize() { 
		auto& services = ServiceLocator::Get();
		Home = &services.GetRequired<HomeBase>();
        return true; 
    }

    void ShutDown() {
        Home = nullptr;
    }

    void Render() { 
        RenderContent();
    }
} // namespace Walker::WalkerUi::Screens::Crew
