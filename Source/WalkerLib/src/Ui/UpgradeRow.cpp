#include "Walker/Ui/UpgradeRow.h"
#include "Walker/Ui/WalkerRebirthPallete.h"
#include "Walker/WalkerRates.h"
#include "Walker/Rebirth/WalkerProgression.h"

#include <imgui.h>

#include <format>
#include <limits>
#include <string>

namespace {
	constexpr Walker::UpgradeLevel MaxLevel = std::numeric_limits<Walker::UpgradeLevel>::max();
	constexpr Walker::ProgressionPoints UnavailableCost = std::numeric_limits<Walker::ProgressionPoints>::max();

	std::string FormatPoints(Walker::ProgressionPoints points) {
		auto value = Walker::Quantity{points};
		return value.ToHumanReadable(2).value_or(value.ToScientific(2));
	}
}

namespace Walker::WalkerUi {
	bool RenderUpgradeRow(const char* label, const UpgradeKey& key, WalkerRates& rates) {
		auto changed = false;
		ImGui::PushID(key.GetIndex());
		if (ImGui::BeginTable("UpgradeTable", 3, ImGuiTableFlags_SizingStretchSame)) {
			auto MakeButton = [&](const char* layerId, WalkerProgression& progression, ImU32 color) {
				ImGui::TableNextColumn();
				ImGui::PushID(layerId);
				auto level = progression.GetLevel(key);
				auto points = progression.GetAvailablePoints();
				auto capped = level == MaxLevel;
				auto unavailableCost = UnavailableCost;
				auto cost = capped ? unavailableCost : rates.GetNextLevelCost(key, level);
				auto priceText = capped
					? std::string{ "Max Level" }
					: cost == unavailableCost
						? std::string{ "Unavailable" }
						: FormatPoints(cost);

				ImGui::BeginDisabled(points < cost || capped || cost == unavailableCost);
				ImGui::PushStyleColor(ImGuiCol_Button, color);
				auto buttonText = priceText + "###Buy";
				if (ImGui::Button(buttonText.c_str(), ImVec2{-1.f, 0.f}) && progression.TryPurchase(key, 1, cost)) {
					changed = true;
				}
				ImGui::PopStyleColor();
				ImGui::EndDisabled();
				ImGui::PopID();
			};
			MakeButton("Rebirth", rates.GetRebirth(), RebirthColor);
			MakeButton("Prestige", rates.GetPrestige(), PrestigeColor);
			MakeButton("Ascend", rates.GetAscend(), AscendColor);
			ImGui::EndTable();
		}

		ImGui::PopID();
		return changed;
	}
}