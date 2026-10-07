#include "Walker/Ui/Screens/MilestonesScreen.h"
#include "Walker/Home/HomeBase.h"
#include "Walker/Milestones/Milestones.h"

#include <Ui/UiUtil.h>
#include <imgui.h>

#include <algorithm>
#include <format>
#include <optional>
#include <string>

namespace {
    using namespace Walker;
	MilestoneKind SelectedMilestone{ MilestoneKind::Unset };
    HomeBase* Home{nullptr};
    MilestoneManager* Milestones{nullptr};

    f32 GetProgress(const MilestoneDetails& details) {
		if (details.Thresholds.empty() || !details.GetValue) return 0.f;
		auto level = Milestones->GetUnlockedTier(details.Kind);
		if (level >= details.Thresholds.size()) return 1.f;

        auto current = details.GetValue(Home->Stats.AllTime());
		auto target = details.Thresholds[level];

		return target > Zero
			? std::clamp(static_cast<f32>(Quantity::Ratio(current, target)), 0.f, 1.f)
			: 1.f;
    }

    void RenderCard(MilestoneKind kind, const char* subtitle) {
        const auto& presentation = GetMilestonePresentation(kind);
        auto unlocked = Milestones->IsUnlocked(kind);
        auto hidden = presentation.Secret && !unlocked;
        auto selected = SelectedMilestone == kind;

        auto label = hidden ? "?" : presentation.Name;
		if (!hidden && subtitle && subtitle[0] != '\0') {
            label += "\n";
            label += subtitle;
		}
        label += "###Card";

        ImGui::PushID(static_cast<int>(kind));
		auto size = ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() * 3.f };
        PushFittedFont(FontSizes::H3, label.c_str(), size, std::nullopt, 2.f);

        ImGui::PushStyleColor(ImGuiCol_Button, unlocked 
            ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive) 
            : ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_Border, selected
			? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)
			: ImGui::GetStyleColorVec4(ImGuiCol_Border));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, selected ? 2.f : 1.f);

        if(ImGui::Button(label.c_str(), size)) {
			SelectedMilestone = selected ? MilestoneKind::Unset : kind;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        ImGui::PopFont();
        ImGui::PopID();
    }

    void RenderTieredMilestones() {
        ImGui::SeparatorText("Tiered Milestones");

		if (ImGui::BeginTable("TieredCards", 4, ImGuiTableFlags_SizingStretchSame)) {
			for (const auto& details : GetTieredMilestones()) {
				ImGui::TableNextColumn();
				auto text = std::format("{}/{}", Milestones->GetUnlockedTier(details.Kind), details.Thresholds.size());
				RenderCard(details.Kind, text.c_str());
                auto height = std::max(2.f, ImGui::GetFontSize() * 0.25f);
                ImGui::ProgressBar(GetProgress(details), ImVec2{ -1.f, height }, "");
			}
			ImGui::EndTable();
		}
    }

    void RenderOneTimeMilestones() {
        ImGui::SeparatorText("One-Time Milestones");

		if (ImGui::BeginTable("OneTimeCards", 4, ImGuiTableFlags_SizingStretchSame)) {
			for (const auto& details : GetOneTimeMilestones()) {
				ImGui::TableNextColumn();
				RenderCard(details.Kind, Milestones->IsUnlocked(details.Kind) ? "Unlocked" : "Locked");
			}
		    ImGui::EndTable();
	    }
    }

    void RenderSelectedDetails() {
        ImGui::SeparatorText("Details");

		if (SelectedMilestone == MilestoneKind::Unset) {
            ImGui::TextUnformatted("Select a milestone");
            return;
        }

		const auto& presentation = GetMilestonePresentation(SelectedMilestone);
		auto level = Milestones->GetUnlockedTier(SelectedMilestone);
		if (presentation.Secret && level == 0) {
			ImGui::TextUnformatted("???");
			return;
		}

        ImGui::TextUnformatted(presentation.Name.c_str());
		if (auto* details = TryGetMilestoneDetails(SelectedMilestone)) {
			auto total = details->Thresholds.size();
            if(total == 0) {
                ImGui::TextUnformatted("No tiers");
                return;
            }

			ImGui::Text("Levels unlocked: %zu/%zu", level, total);
            auto complete = level >= total;
            auto tier = complete ? total : level + 1;
			auto description = DescribeMilestone(SelectedMilestone, tier);

			ImGui::TextUnformatted(complete ? "Complete" : "Next Level:");
			ImGui::TextWrapped("%s", description.c_str());

            if(!complete && details->GetValue) {
				auto progress = GetProgress(*details);
                ImGui::ProgressBar(progress, ImVec2{-1.f, 0.f}, "");
            }
		} else {
			ImGui::TextUnformatted(level > 0 ? "Unlocked" : "Locked");
			auto description = DescribeMilestone(SelectedMilestone, 1);
			ImGui::TextWrapped("%s", description.c_str());
        }

        if(!presentation.BenefitDescription.empty()) {
			ImGui::TextWrapped("Benefit: %s", presentation.BenefitDescription.c_str());
        }
    }
}

namespace Walker::WalkerUi::Screens::Milestones {
    bool Initialize() {
		Home = &ServiceLocator::Get().GetRequired<HomeBase>();
		::Milestones = &Home->Milestones;
        return true;
    }

    void ShutDown() {
        ::Milestones = nullptr;
    }

    void Render() {
		RenderTieredMilestones();
		RenderOneTimeMilestones();
		RenderSelectedDetails();
    }
}
